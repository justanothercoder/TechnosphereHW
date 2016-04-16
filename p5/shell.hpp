#pragma once

#include <memory>
#include <string>
#include "processtree.hpp"

class Shell
{
public:
    void run();
    std::unique_ptr<ProcessTree> parse(const std::string& line);
};
