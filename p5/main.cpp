#include <iostream>
#include "shell.hpp"

int main()
{
    Shell shell;
    global_shell = &shell;
    shell.run();
}
