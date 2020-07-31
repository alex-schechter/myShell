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
    add_job_to_list(&stopped_jobs, STOPPED);
}

void CONThandler(int sig){
    (void)sig;
    printf("got CONT\n");
    // add_job_to_list(&stopped_jobs, STOPPED);
}

void CHLDhandler(int sig){
    (void)sig;
    // char message[MAX_SIZE];
    // message[MAX_SIZE-1] = '\0';
    // snprintf(message, MAX_SIZE, "process %d, %d, %d is done\n", pid, getpid(), getppid());
    // write(STDOUT_FILENO, message, strlen(message)+1);
}