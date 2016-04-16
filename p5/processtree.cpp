#include "processtree.hpp"
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

LeafProcess::LeafProcess(Command command) : command(command) { }

int LeafProcess::run()
{
    std::unique_ptr<const char*[]> args(new const char*[command.args.size() + 2]);

    args[0] = command.program.data();
    for (int i = 0; i < command.args.size(); ++i) {
        args[i + 1] = command.args[i].data();
    }
    args[command.args.size() + 1] = nullptr;    

    if (command.in_file != "") {
        int fd = open(command.in_file.data(), O_RDONLY);
        dup2(fd, 0);
        close(fd);
    }
    
    if (command.out_file != "") {
        int fd = open(command.out_file.data(), O_CREAT | O_WRONLY | O_TRUNC, 0755);
        dup2(fd, 1);
        close(fd);
    }

    execvp(command.program.data(), const_cast<char* const*>(static_cast<const char* const*>(args.get())));
    exit(1);
}

PipeProcess::PipeProcess(Tree from, Tree to) : from(std::move(from)), to(std::move(to)) { }

int PipeProcess::run()
{
    int p[2];
    pipe(p);

    if (fork() == 0) {
        dup2(p[1], 1);
        close(p[1]);
        from->run();
    }

    if (fork() == 0) {
        dup2(p[0], 0);
        close(p[0]);
        to->run();
    }

    close(p[0]);
    close(p[1]);

    while (wait(nullptr) != -1) { }
    exit(0);
}

AndProcess::AndProcess(Tree left, Tree right) : left(std::move(left)), right(std::move(right)) { }

int AndProcess::run()
{
    if (fork() == 0) {
        left->run();
    }
    int status;
    wait(&status);
    status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;

    exit(status ? status : right->run());
}

OrProcess::OrProcess(Tree left, Tree right) : left(std::move(left)), right(std::move(right)) { }

int OrProcess::run()
{
    if (fork() == 0) {
        left->run();
    }
    int status;
    wait(&status);
    status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    exit(status ? right->run() : status);
}
