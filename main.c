#include "funcs.h"

int main(int argc, char **argv, char **env){

    char **commands;
    char *buffer;
    char *env_command="env", *exit_command="exit", *dolar="$ ";
    ssize_t characters; size_t length;
    struct stat check_file;  
    int background = 0;
    int pid, status;

    buffer = NULL;
    length = 0;

    signal (SIGINT, INThandler);
    // signal (SIGCHLD, CHILDhandler);

    write(STDOUT_FILENO, dolar, strlen(dolar));

    printf("you can write now\n");

    while((characters = getline(&buffer, &length, stdin))){


        if (characters == EOF){
            exit(EXIT_FAILURE);
        }

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
            write(STDOUT_FILENO, dolar, strlen(dolar));
            buffer = NULL;
            background = 0;
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
            // if (background == 1){
            //     setpgid(0, 0);
            // }
        
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
            // int parentPID = getpid();
            // setpgid(0, 0);


            if (background == 1){
                // int length = snprintf( NULL, 0, "%d", pid );
                // char *p = malloc(length + 4 + 1);
                // char *str = malloc( length + 2 );
                // if (str == NULL || p == NULL){
                //     perror("could not allocate memory");
                //     free(buffer);
                //     free_duble_ptr(commands);
                //     exit(EXIT_FAILURE);
                // }
                // snprintf( str, length + 1, "%d", pid );
                // strcat(p, "PID ");
                // strcat(p, str);
                // write(STDOUT_FILENO, p, strlen(p));
                // write(STDOUT_FILENO, "\n", 1);

                // setpgrp();
                int stat;
                waitpid(-1, &stat, WNOHANG);
            }

            else {
                wait(NULL);
            }
            

            if (commands == NULL){
                free(buffer);
                free_duble_ptr(commands);
            }

            //check if the command is exit
            else if (strcmp(commands[0], exit_command) == 0){
                exit_cmd(buffer, commands);
            }

            else{
                free(buffer);
                free_duble_ptr(commands);
            }

        }

        write(STDOUT_FILENO, dolar, strlen(dolar));
        buffer = NULL;
        background = 0;
        printf("you can write now\n");

        
    }

    return 0;
}