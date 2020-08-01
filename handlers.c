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
    printf("got signal\n");
    add_job_to_list(&stopped_jobs, STOPPED);
}

void CONThandler(int sig){
    (void)sig;
    // add_job_to_list(&stopped_jobs, STOPPED);
}

void CHLDhandler(int sig){
    (void)sig;
    print_shell("$ ");

}