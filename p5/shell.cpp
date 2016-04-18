#include "shell.hpp"
#include "commandparser.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

Shell* global_shell = nullptr;

void sigint_handler(int signum)
{
    signal(SIGINT, sigint_handler);
    std::cerr << "SIGINT handler\n" << "Current pid: " << global_shell->current_pid << '\n';

    if (global_shell->current_pid != 0) {
        kill(global_shell->current_pid, SIGINT);
//        waitpid(global_shell->current_pid, nullptr, 0);
    }
}
    
void Shell::run()
{
    signal(SIGINT, sigint_handler);
    std::set<pid_t> daemon_pids;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;            
        }

        auto tree = parse(line);
        
        pid_t pid;
        if ((pid = fork()) == 0) {
//            signal(SIGINT, SIG_DFL);
            tree->run();
        } else {
            if (!tree->is_daemon) {
                int status;
                current_pid = pid;
                if (waitpid(pid, &status, 0) <= 0) {
                    continue;
                }
                current_pid = 0;
            } else {
                daemon_pids.emplace(pid);
                std::cerr << "Spawned child process " << pid << '\n';
            }
        }

        for (auto it = daemon_pids.begin(); it != daemon_pids.end(); ) {
            auto pid = *it;
            int status;
            if (pid == waitpid(pid, &status, WNOHANG)) {
                std::cerr << "Process " << pid << " exited: " << (WIFEXITED(status) ? WEXITSTATUS(status) : 1) << '\n';
                it = daemon_pids.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::unique_ptr<ProcessTree> Shell::parse(const std::string& line)
{
    CommandParser parser;
    return parser.parse(line);
}
