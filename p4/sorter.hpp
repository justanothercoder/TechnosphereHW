#pragma once

#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <map>

class Sorter
{
    using DocID = std::uint64_t;
    using WordID = std::uint64_t;

    using offset_t = std::uint64_t;

public:
    void sort(const std::string& filename, const std::string& save_to);
    void dumpPairsToFile(std::vector<std::pair<WordID, DocID>> pairs);
    void mergeBlocks(const std::string& save_to);

private:
    std::ofstream temp;
    std::vector<std::pair<offset_t, offset_t>> block_offset;
    std::map<WordID, DocID> doc_count;
};
