#include "funcs.h"

extern pid_t pid;
extern job *stopped_jobs;

// Ctrl C handler
void INThandler (int sig)
{
    (void)sig;
    printf("sdfsdfsdfasfd\n");
    write(STDOUT_FILENO, "\n", 2);
    print_shell("$ ");
}

// Ctrl Z handler
void TSTPhandler(int sig){
    (void)sig;
    // signal(SIGTSTP, SIG_DFL);
    // killpg(getpgrp(), SIGTSTP);
    // printf("adding process to list: %d\n", pid);
    // add_job_to_list(&stopped_jobs, STOPPED);
    // print_shell("$ ");
}

void CONThandler(int sig){
    (void)sig;
    // add_job_to_list(&stopped_jobs, STOPPED);
}

void CHLDhandler(int sig){
    (void)sig;
    // do_job_notification();
    // while ((pid=waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0)
    // {
    //    printf("process %d died\n", pid);
    //    job *j = find_job_by_pid(stopped_jobs, pid);
    //    if (j){
    //         printf("removing job from list: %d\n", j->job_num);
    //         remove_job_from_list(&stopped_jobs, j->job_num);
    //    }
        // 
    // }

}