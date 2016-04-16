#pragma once

/*
 * Shell
 * Архитектура: Linux
 *
 */

#include <memory>
#include <string>
#include <set>
#include "processtree.hpp"

class Shell
{
public:
    void run();
    std::unique_ptr<ProcessTree> parse(const std::string& line);

    pid_t current_pid;
};

extern Shell* global_shell;
