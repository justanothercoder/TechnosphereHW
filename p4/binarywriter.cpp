#include "binarywriter.hpp"

BinaryWriter::BinaryWriter(const std::string& filename) : out(filename, std::ios::binary)
{

}

void BinaryWriter::seek(std::uint64_t offset)
{
    out.seekp(offset, out.beg);
}

void BinaryWriter::write(std::uint64_t n)
{
    out.write(reinterpret_cast<char*>(&n), sizeof(n));
}

void BinaryWriter::write32Int(std::uint32_t n)
{
    out.write(reinterpret_cast<char*>(&n), sizeof(n));
}

std::uint64_t BinaryWriter::offset() 
{
    return out.tellp();
}
