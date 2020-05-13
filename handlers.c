#include "funcs.h"

void INThandler (int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n", 2);
    print_shell("$ ");
}

void TSTPhandler(int sig){
    (void)sig;
    char message[MAX_SIZE];
    message[MAX_SIZE-1] = '\0';
    snprintf(message, MAX_SIZE, "id %d stopped\n", getpid());
    write(STDOUT_FILENO, message, strlen(message)+1);
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    print_shell("$ ");
    // setpgid(0,0);
    
}

void CONThandler(int sig){
    printf("*********\n");
    // kill(getpid(), SIGCONT);
}