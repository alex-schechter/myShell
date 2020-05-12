#include "funcs.h"

void INThandler (int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n$ ", 3);
}

void CHILDhandler(int sig){
    (void)sig;
    
    // write(STDOUT_FILENO, "\n", 1);
}