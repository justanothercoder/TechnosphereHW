#pragma once

#include <cstdint>
#include <fstream>

class BinaryWriter
{
public:
    BinaryWriter(const std::string& filename);
    void seek(std::uint64_t offset);    

    void write(std::uint64_t n);
    std::uint64_t offset();

private:
    std::ofstream out;
};
