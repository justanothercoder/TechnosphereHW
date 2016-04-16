#pragma once

#include <fstream>
#include <map>
#include <vector>

class BinaryReader
{
public:
    BinaryReader(const std::string& filename);
    std::map<std::uint64_t, std::vector<std::uint64_t>> getBatch(std::size_t batch_size);

private:
    std::ifstream in;
    int state = 0;
    std::uint64_t current_docid;
    std::uint32_t number_of_words;
    std::uint32_t read_words;
};
