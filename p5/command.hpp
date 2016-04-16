#pragma once

#include <string>
#include <vector>

class Command
{
public:
    std::string program;
    std::vector<std::string> args;
    
    std::string in_file, out_file;
};
