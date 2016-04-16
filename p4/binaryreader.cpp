#include "binaryreader.hpp"

BinaryReader::BinaryReader(const std::string& filename) : in(filename, std::ios::binary)
{

}

std::map<std::uint64_t, std::vector<std::uint64_t>> BinaryReader::getBatch(std::size_t batch_size)
{
    std::map<std::uint64_t, std::vector<std::uint64_t>> result;
    
    std::size_t read = 0;
    while (read < batch_size) {
       if (state == 0) { // Reading doc_id
           if ( !in.read(reinterpret_cast<char*>(&current_docid), sizeof(current_docid)) 
                   .read(reinterpret_cast<char*>(&number_of_words), sizeof(number_of_words)) ) {
               break;
           }
           read_words = 0;
           state = 1;
       } else if (state == 1) { // Reading words
           if (read_words == number_of_words) {
               state = 0;
           } else {
               std::uint64_t doc_id;
               in.read(reinterpret_cast<char*>(&doc_id), sizeof(doc_id));
               ++read_words;
               read += sizeof(doc_id);
               result[current_docid].emplace_back(doc_id);
           }
       }
    }

    return std::move(result);
}
