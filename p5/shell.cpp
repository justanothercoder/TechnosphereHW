#include "shell.hpp"
#include "commandparser.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void Shell::run()
{
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;            
        }

        auto tree = parse(line);

        pid_t pid;
        if ((pid = fork()) == 0) {
            tree->run();
        } else {
            if (!tree->is_daemon) {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
}

std::unique_ptr<ProcessTree> Shell::parse(const std::string& line)
{
    CommandParser parser;
    return parser.parse(line);
}
