#include "sorter.hpp"
#include "binaryreader.hpp"
#include "binarywriter.hpp"
#include <algorithm>

#include <iostream>

void Sorter::sort(const std::string& filename, const std::string& save_to)
{
    BinaryReader reader(filename);

    temp.open("temp");
    
    std::vector<std::pair<WordID, DocID>> pairs;
    std::map<DocID, std::vector<WordID>> words;

    std::size_t batch_size = 1024 * 1024 * 512; // half gigabyte
    while (!(words = reader.getBatch(batch_size)).empty()) {
        for (auto doc_id_and_words: words) {
            auto doc_id = doc_id_and_words.first;
            for (auto word_id: doc_id_and_words.second) {
                doc_count[word_id] += 1;
                pairs.emplace_back(word_id, doc_id);
            }
        }
       
        std::sort(pairs.begin(), pairs.end());

        dumpPairsToFile(std::move(pairs));
        pairs.clear();
    }
    temp.close();

    mergeBlocks(save_to);
}

void Sorter::dumpPairsToFile(std::vector<std::pair<WordID, DocID>> pairs)
{
    offset_t start = temp.tellp(), end;
    for (const auto& word_id_doc_id: pairs) {
        temp << word_id_doc_id.first << ' ' << word_id_doc_id.second << '\n';
    }
    end = temp.tellp();

    block_offset.emplace_back(start, end);
}
    
void Sorter::mergeBlocks(const std::string& save_to)
{
    BinaryWriter writer(save_to);
    BinaryWriter dict_writer(save_to);

    writer.seek((doc_count.size() + 1) * 2 * sizeof(std::uint64_t));
    
    std::ifstream in("temp");

    std::vector<offset_t> offsets;
    std::vector<std::pair<WordID, DocID>> values;

    for (auto off: block_offset) {
        WordID word_id;
        DocID doc_id;
    
        in.seekg(off.first, in.beg);
        in >> word_id >> doc_id;
        
        offsets.push_back(in.tellg());
        
        values.emplace_back(word_id, doc_id);
    }
    
    WordID last_word_id = 0;

    while (true) {
        int min_ind = -1;

        for (std::size_t i = 0; i < values.size(); ++i) {
            if (offsets[i] != block_offset[i].second) {
                if (min_ind == -1 || values[i] < values[min_ind]) {
                    min_ind = i;
                }
            }
        }

        if (min_ind == -1) {
            break;
        }

        WordID word_id;
        DocID doc_id;

        std::tie(word_id, doc_id) = values[min_ind];

        if (word_id != last_word_id) {
            last_word_id = word_id;
            dict_writer.write(word_id);
            dict_writer.write(writer.offset());

            writer.write32Int(doc_count[word_id]);
        }
        writer.write(doc_id);

        in.seekg(offsets[min_ind], in.beg);
        if ( !(in >> values[min_ind].first >> values[min_ind].second) ) {
            break;
        }

        offsets[min_ind] = in.tellg();
    }

    dict_writer.write(0);
    dict_writer.write(0);
}
