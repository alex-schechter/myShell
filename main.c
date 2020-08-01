#include "funcs.h"

pid_t pid;
pid_t shell_pid;

job *stopped_jobs;
char *buff;

int main(int argc, char **argv, char **env){

    char **commands;
    process *commandsSplitedByPipes;
    char *buffer;
    char *env_command="env", *exit_command="exit", *dolar="$ ";
    ssize_t characters; 
    size_t length;
    struct stat check_file;  
    int background = 0;
    int status;

    /* Put our shell in its own process group in order to be
       placed in the fourground in our parent shell to enable job control */
    shell_pid = getpid();
    if (setpgid (shell_pid, shell_pid) < 0){
          perror ("Couldn't put the shell in its own process group");
          exit (1);
    }

    // Get vontroll of the terminal
    tcsetpgrp(0, shell_pid);

    buffer = NULL;
    length = 0;

    // signal handlers
    signal (SIGINT, INThandler);    
    signal (SIGTSTP, TSTPhandler);
    signal (SIGCONT, CONThandler);
    signal (SIGCHLD, CHLDhandler);

    // ignore signals
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);

    print_shell(dolar);

    while((characters = getline(&buffer, &length, stdin))){

        if (characters == EOF){
            exit(EXIT_FAILURE);
        }

        if (strncmp(buffer, "\n", strlen(buffer)) == 0){
            free(buffer);
            buffer = NULL;
            print_shell("$ ");
            continue;
        }

        //check if it is a background process
        int len = _strlen(buffer);
        buffer[len] = '\0';
        if ((char)buffer[len-2] == '&'){
            background = 1;
            buffer[len-2] = '\n';
            buffer[len-1] = '\0';
        }

        int numPipes = get_number_of_pipes(buffer);
        int *pipefds = malloc(2*numPipes);
        if (pipefds == NULL){
            perror("could not allocate pipes");
            free(buffer);
            exit(EXIT_FAILURE);
        }

        int filedes[2]; // pos. 0 output, pos. 1 input of the pipe
	    int filedes2[2];

        commandsSplitedByPipes = split_by_pipe(buffer);

        if (commandsSplitedByPipes == NULL){
            printf("could not background the command if it is not the last one\n");
            print_shell(dolar);
            buffer = NULL;
            background = 0;
            free(buff);
            buff = NULL;
            continue;
        }

        int j=0;

        while (commandsSplitedByPipes != NULL){
            //parse the commands from input
            char *copy = strdup(commandsSplitedByPipes->command);
            if (copy == NULL){
                print_shell(dolar);
                buffer = NULL;
                background = 0;
                free(buff);
                buff = NULL;
                break;
            }
            commands = parse_commands(copy);
            if (commands == NULL){
                print_shell(dolar);
                free(buffer);
                buffer = NULL;
                background = 0;
                free(buff);
                buff = NULL;
                free_processes(commandsSplitedByPipes);
                break;
            }

            if (numPipes > 0){
                // for odd j
                if (j % 2 != 0){
                    pipe(filedes); 
                }
                // for even j
                else{
                    pipe(filedes2); 
                }
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

                if (numPipes > 0){

                    // if this is the first command
                    if (j==0){
                        dup2(filedes2[1], STDOUT_FILENO);
                    }

                    // if it is the last command
                    else if (commandsSplitedByPipes->next == NULL){
                        // for odd number of commands
                        if ((numPipes+1) % 2 != 0){ 
                            dup2(filedes[0],STDIN_FILENO);
                        }
                        // for even number of commands
                        else{ 
                            dup2(filedes2[0],STDIN_FILENO);
                        }
                    // if it is a middle command
                    }
                    else{ 
                        // for odd j
                        if (j % 2 != 0){
                            dup2(filedes2[0],STDIN_FILENO); 
                            dup2(filedes[1],STDOUT_FILENO);
                        // for even j
                        }else{ 
                            dup2(filedes[0],STDIN_FILENO); 
                            dup2(filedes2[1],STDOUT_FILENO);					
                        } 
                    }
                }

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
                    int job_num = check_job_number(commands[1]);
                    if (job_num == -1){
                        printf("no such job: %s", commands[1]);
                        exit(EXIT_FAILURE);
                    }
                    continue_job(&stopped_jobs, job_num);
                    exit(EXIT_SUCCESS);
                }

                //check if the command is bg
                else if(strcmp(commands[0], "bg") == 0){
                    int job_num = check_job_number(commands[1]);
                    if (job_num == -1){
                        printf("no such job: %s", commands[1]);
                        exit(EXIT_FAILURE);
                    }
                    continue_job(&stopped_jobs, job_num);
                    exit(EXIT_SUCCESS);
                }

                //check if the command is jobs
                else if(strcmp(commands[0], "jobs") == 0){
                    int job_num = check_job_number(commands[1]);
                    if (job_num == -1){
                        printf("no such job: %s\n", commands[1]);
                        exit(EXIT_FAILURE);
                    }
                    print_jobs(stopped_jobs, job_num);
                    exit(EXIT_SUCCESS);
                }

                //check if the command is a full path to command
                else if (stat(commands[0], &check_file) == 0){
                    if (execvp(commands[0], commands) < 0){
                        perror("error in execvp\n");
                        exit(EXIT_FAILURE);
                    }
                }

                //need to search the command in every directory from PATH and if is there run it
                else{
                    search_in_path(commands, env);
                }
                
            }

            //father process
            else{

                // close relevant pipes
                if (numPipes > 0){
                    if (j == 0){
                        close(filedes2[1]);
                    }
                    else if (commandsSplitedByPipes->next == NULL){
                        if ((numPipes+1) % 2 != 0){					
                            close(filedes[0]);
                        }else{					
                            close(filedes2[0]);
                        }
                    }else{
                        if (j % 2 != 0){					
                            close(filedes2[0]);
                            close(filedes[1]);
                        }else{					
                            close(filedes[0]);
                            close(filedes2[1]);
                        }
                    }
                }

                if (background == 0){
                    pid = waitpid(-1, &status, WUNTRACED);
                    
                }

                else{
                    add_job_to_list(&stopped_jobs, RUNNING);
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
                    int job_num = check_job_number(commands[1]);
                    if (job_num == -1){
                        printf("wrong job id: %s", commands[1]);
                        exit(EXIT_FAILURE);
                    }
                     
                    remove_job_from_list(&stopped_jobs, job_num);
                    pid = waitpid(-1, &status, WUNTRACED);
                }

                //check if the command is fg
                else if(strcmp(commands[0], "bg") == 0){
                    int job_num = check_job_number(commands[1]);
                    if (job_num == -1){
                        printf("wrong job id: %s", commands[1]);
                        exit(EXIT_FAILURE);
                    }
                    remove_job_from_list(&stopped_jobs, job_num);
                }

                else{
                    free(buffer);
                    free_duble_ptr(commands);
                }

            }
            
            process *temp = commandsSplitedByPipes;
            free(temp);
            temp = NULL;
            commandsSplitedByPipes = commandsSplitedByPipes->next;
            j++;
        }
        print_shell(dolar);
        buffer = NULL;
        background = 0;
        free(buff);
        buff = NULL;
        free(commandsSplitedByPipes);
        commandsSplitedByPipes = NULL;
    }

    return 0;
}