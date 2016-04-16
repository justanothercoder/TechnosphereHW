#include <iostream>
#include "sorter.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "No arguments provided\n";
        return 1;
    }

    Sorter sorter;
    sorter.sort(argv[1], "sorted_index.bin");
}
