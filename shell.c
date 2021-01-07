#include "funcs.h"

extern pid_t shell_pgid;
extern int shell_terminal;
extern int shell_is_interactive;
extern struct termios shell_tmodes;

void init_shell(){
    /* Check if we are running interactively */
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty (shell_terminal); 
    if (shell_is_interactive) {
        /* Loop until we are in the foreground (while the real shell's pgrp is not our new pgrp) */
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
          kill (- shell_pgid, SIGTTIN); 
        /* Ignore interactive and job-control signals. */
        signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, SIG_IGN);  

        /* Put ourselves in our own process group */
        shell_pgid = getpid();
        if (setpgid (shell_pgid, shell_pgid) < 0)
          {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
          } 
        /* Get control of the terminal */
        tcsetpgrp (shell_terminal, shell_pgid);

        /* Save default terminal attributes for shell */
        tcgetattr (shell_terminal, &shell_tmodes);
    }
}


/* Put job j in the foreground  
   If cont is nonzero, restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block */
void put_job_in_foreground (job *j, int cont) {
    /* Put the job into the foreground */
    tcsetpgrp (shell_terminal, j->pgid);

    /* Send the job a continue signal, if necessary */
    if (cont)
    {
        printf("%s", j->command);
        tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
        if (kill (- j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }

    /* Wait for it to report */
    wait_for_job (j);

    /* Put the shell back in the foreground */
    tcsetpgrp (shell_terminal, shell_pgid);

    /* Restore the shell’s terminal modes */
    tcgetattr (shell_terminal, &j->tmodes);
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}


/* Put a job in the background  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up */
void put_job_in_background (job *j, int cont) {
    /* Send the job a continue signal, if necessary */
    if (cont) {
        if (kill (-j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }
}