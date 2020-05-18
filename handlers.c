#include "funcs.h"

extern pid_t pid;
extern job *stopped_jobs;

// Ctrl C handler
void INThandler (int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n", 2);
    print_shell("$ ");
}

// Ctrl Z handler
void TSTPhandler(int sig){
    (void)sig;
    // printf("the address of stopped_jobs before is %p\n", stopped_jobs);
    add_job_to_list(&stopped_jobs, "STOPPED");
    // printf("the address of stopped_jobs after is %p\n", stopped_jobs);
}

void CONThandler(int sig){
    (void)sig;
    printf("*********\n");
    // kill(getpid(), SIGCONT);
}