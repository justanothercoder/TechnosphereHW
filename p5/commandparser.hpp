#pragma once

#include <memory>
#include <string>
#include "processtree.hpp"

class CommandParser
{
public:
    Tree parse(const std::string& line);

private:
    void tokenize(const std::string& line);
    Tree parse_command();

    bool isNotOperation(const std::string& line);

    std::vector<std::string> tokens;
    int pos = 0;
};
