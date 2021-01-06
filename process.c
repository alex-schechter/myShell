#include "funcs.h"

extern int shell_terminal;
extern int shell_is_interactive;
extern job *first_job;

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground){
    pid_t pid;

    if (shell_is_interactive)
    {
        /* Put the process into the process group and give the process group
            the terminal, if appropriate.
            This has to be done both by the shell and in the individual
            child processes because of potential race conditions */
        pid = getpid ();
        if (pgid == 0) 
            pgid = pid;

        setpgid (pid, pgid);
        
        /* If this is a fourground process put it in the shell's foreground */
        if (foreground)
            tcsetpgrp (shell_terminal, pgid);

        /* Set the handling for job control signals back to the default */
        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);
    }

    /* Set the standard input/output channels of the new process */
    if (infile != STDIN_FILENO)
    {
        dup2 (infile, STDIN_FILENO);
        close (infile);
    }
    if (outfile != STDOUT_FILENO)
    {
        dup2 (outfile, STDOUT_FILENO);
        close (outfile);
    }
    if (errfile != STDERR_FILENO)
    {
        dup2 (errfile, STDERR_FILENO);
        close (errfile);
    }

    /* Exec the new process make sure we exit */    
    execvp (p->argv[0], p->argv);
    perror ("");
    exit (EXIT_FAILURE);
}

/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise */
int mark_process_status (pid_t pid, int status) {
    job *j;
    process *p;
    if (pid > 0) {
        for (j = first_job; j; j = j->next){
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid)
                {
                    p->status = status;
                    if (WIFSTOPPED (status)) {
                        printf("process stopped\n");
                        p->stopped = 1;
                    }
                    else {
                        printf("process finished\n");
                        p->finished = 1;
                        if (WIFSIGNALED (status))
                            fprintf (stderr, "%d: Terminated by signal %d.\n",
                                        (int) pid, WTERMSIG (p->status));
                    }
                    return 0;
                }
        }
        fprintf (stderr, "No child process %d.\n", pid);
        return -1;
    }

    /* No processes ready be reported */
    else if (pid == 0 || errno == ECHILD)
        return -1;

    /* Other errors */
    else {
        perror ("waitpid");
        return -1;
    }
}

/* Check for processes that have status information available, without blocking */
void update_status (void) {
    int status;
    pid_t pid;

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
    while (!mark_process_status (pid, status));
}