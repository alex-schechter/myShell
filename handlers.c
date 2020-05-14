#include "funcs.h"

extern pid_t pid;
extern jobList stopped_jobs;

void INThandler (int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n", 2);
    print_shell("$ ");
}


void TSTPhandler(int sig){
    (void)sig;
    int last_index;
    char message[MAX_SIZE];
    message[MAX_SIZE-1] = '\0';

    add_job_to_list(&stopped_jobs, "STOPPED");
    last_index = stopped_jobs.size-1;
    snprintf(message, MAX_SIZE, "[%d]+ %s %s\n", stopped_jobs.jobs[last_index].job_num, stopped_jobs.jobs[last_index].status, stopped_jobs.jobs[last_index].command);
    write(STDOUT_FILENO, message, strlen(message)+1);

}

void CONThandler(int sig){
    (void)sig;
    printf("*********\n");
    // kill(getpid(), SIGCONT);
}