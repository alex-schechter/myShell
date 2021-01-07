#include "funcs.h"

job *stopped_jobs;
char *buff;

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;
job *first_job = NULL;
int job_count = 0;


int main(int argc, char **argv, char **env){

    process *process_list;
    char *buffer;
    const char *dolar = "$ ";
    ssize_t characters; 
    size_t length;
    int background = 0;

    /* Put our shell in its own process group in order to be
       placed in the fourground in our parent shell to enable job control */
    init_shell();

    buffer = NULL;
    length = 0;

    // // signal handlers
    // signal (SIGINT, INThandler);    
    // signal (SIGTSTP, TSTPhandler);
    // signal (SIGCONT, CONThandler);
    signal (SIGCHLD, CHLDhandler);

    // // ignore signals
    // signal (SIGTTIN, SIG_IGN);
    /* signal (SIGTTOU, SIG_IGN); */

    

    /* The main loop of the shell */
    while (1) {
        print_shell("$ ");
        /* Get input from user */
        characters = getline(&buffer, &length, stdin);

        /* End of line */
        if (characters == EOF)
            exit(EXIT_FAILURE);

        int len = strlen(buffer);
        buffer[len] = '\0';
        
        /* Check if its a foreground process */
        if ((char)buffer[len-2] == '&') {
            background = 1;
            buffer[len-2] = '\n';
            buffer[len-1] = '\0';
        }

        job *j = create_job_from_command(buffer);
        if (j == NULL)
            continue;

        /* If the job list is empty */
        if (first_job == NULL) {
            first_job = j;
        }
            
        /* If the job list is not empty */
        else {
            
            job *last_job = get_last_job(first_job);
            last_job->next = j;
        }

        job_count += 1;

        launch_job(j, 1-background, env);
        do_job_notification();
    }

    return 0;
}