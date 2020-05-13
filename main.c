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

    print_shell(dolar);

    while((characters = getline(&buffer, &length, stdin))){


        if (characters == EOF){
            exit(EXIT_FAILURE);
        }

        pid = waitpid(-1, &status, WNOHANG);

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
            if (background == 0){
                pid = waitpid(-1, &status, 0);
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

        print_shell(dolar);
        buffer = NULL;
        background = 0;
    }

    return 0;
}