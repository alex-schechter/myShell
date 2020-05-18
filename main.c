#include "funcs.h"

pid_t pid;
pid_t shell_pid;

job *stopped_jobs;
char *buff;

int main(int argc, char **argv, char **env){

    char **commands;
    char *buffer;
    char *env_command="env", *exit_command="exit", *dolar="$ ";
    ssize_t characters; 
    size_t length;
    struct stat check_file;  
    int background = 0;
    int status;

    shell_pid = getpid();
    setpgid(shell_pid, shell_pid);
    tcsetpgrp(0, shell_pid);

    buffer = NULL;
    length = 0;

    // signal handlers
    signal (SIGINT, INThandler);    
    signal (SIGTSTP, TSTPhandler);
    signal (SIGCONT, CONThandler);

    print_shell(dolar);

    while((characters = getline(&buffer, &length, stdin))){

        if (characters == EOF){
            exit(EXIT_FAILURE);
        }

        pid = waitpid(-1, &status, WNOHANG|WUNTRACED);

        int len = _strlen(buffer);

        

        //check if it is a background process
        if ((char)buffer[len-2] == '&'){
            background = 1;
            buffer[len-2] = '\n';
            buffer[len-1] = '\0';
        }

        //parse the commands from input
        commands = parse_commands(buffer);
        if (commands == NULL){
            print_shell(dolar);
            buffer = NULL;
            background = 0;
            free(buff);
            buff = NULL;
            continue;
        }

        //create a new process for the command
        pid = fork();

        //fork failed
        if (pid == -1){
            perror("Error: ");
            exit(EXIT_FAILURE);
        }
        // child process
        if(pid == 0){
            signal(SIGTSTP, SIG_DFL);
            signal(SIGCONT, SIG_DFL);

            if (background){
                setpgid(0,0);
            }
            
            if (commands == NULL){
                commands_is_null(buffer);
            }
            //check if the command is exit
            else if (strcmp(commands[0], exit_command) == 0){
                exit_cmd(buffer, commands);
            }

            //check if the command is env
            else if (strcmp(commands[0], env_command) == 0){
                print_env(buffer, commands, env);
            }

            //check if the command is cd
            else if(strcmp(commands[0], "cd") == 0){
                exit(EXIT_SUCCESS);
            }

            //check if the command is fg
            else if(strcmp(commands[0], "fg") == 0){
                continue_job(&stopped_jobs, commands[1]);
                exit(EXIT_SUCCESS);
            }

            //check if the command is bg
            else if(strcmp(commands[0], "bg") == 0){
                continue_job(&stopped_jobs, commands[1]);
                exit(EXIT_SUCCESS);
            }

            //check if the command is jobs
            else if(strcmp(commands[0], "jobs") == 0){
                print_jobs(stopped_jobs, commands[1]);
                exit(EXIT_SUCCESS);
            }

            //check if the command is a full path to command
            else if (stat(commands[0], &check_file) == 0){
                execve(commands[0], commands, NULL);
            }

            //need to search the command in every directory from PATH and if is there run it
            else{
                search_in_path(commands, env);
            }

        }

        //father process
        else{
            // printf("child pid is: %d\n", pid);
            // printf("father pid is: %d\n", shell_pid);

            if (background == 0){
                // printf("blocking\n");
                pid = waitpid(-1, &status, WUNTRACED);
            }

            if (commands == NULL){
                free(buffer);
                free_duble_ptr(commands);
            }

            //check if the command is exit
            else if (strcmp(commands[0], exit_command) == 0){
                exit_cmd(buffer, commands);
            }

            //check if the command is cd
            else if(strcmp(commands[0], "cd") == 0){
                if (chdir(commands[1])!=0){
                    printf("could not cd into '%s'\n", commands[1]);
                }
            }

            //check if the command is fg
            else if(strcmp(commands[0], "fg") == 0){
                remove_job_from_list(&stopped_jobs, commands[1]);
                waitpid(-1, NULL, 0); 
            }

            //check if the command is fg
            else if(strcmp(commands[0], "bg") == 0){
                remove_job_from_list(&stopped_jobs, commands[1]);
            }

            else{
                free(buffer);
                free_duble_ptr(commands);
            }

            

        }

        print_shell(dolar);
        buffer = NULL;
        background = 0;
        free(buff);
        buff = NULL;
    }

    return 0;
}