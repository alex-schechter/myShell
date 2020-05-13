#include "funcs.h"

void INThandler (int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n", 2);
    print_shell("$ ");
}

void CHILDhandler(int sig){
    (void)sig;
}