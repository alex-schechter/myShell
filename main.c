#include "funcs.h"

pid_t pid;
pid_t shell_pid;

job *stopped_jobs;
char *buff;

int main(int argc, char **argv, char **env){

    process *process_list;
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

        buff = strdup(buffer);
        if (buff == NULL){
            perror("could not aloocate memory\n");
            exit(EXIT_FAILURE);
        }
        if (buff[len-1] == '\n'){
            buff[len-1] = '\0';
        }
        process_list = split_by_pipe(buffer);

        if (process_list == NULL){
            printf("could not background the command if it is not the last one\n");
            print_shell(dolar);
            buffer = NULL;
            background = 0;
            free(buff);
            buff = NULL;
            continue;
        }

        int j=0;

        while (process_list != NULL){

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

                // vars for redirects
                int fd0,fd1,i,in=0,out=0;
                char input[64],output[64];

                if (numPipes > 0){

                    printf("the pid is: %d\n", getpid());
                    printf("the pgid is: %d\n", getpgrp());


                    // if this is the first command
                    if (j==0){
                        dup2(filedes2[1], STDOUT_FILENO);
                    }

                    // if it is the last command
                    else if (process_list->next == NULL){
                        // for odd number of process_list->argv
                        if ((numPipes+1) % 2 != 0){ 
                            dup2(filedes[0],STDIN_FILENO);
                        }
                        // for even number of process_list->argv
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

                for (i=0; process_list->argv[i]!=NULL;i++){
                    if(strcmp(process_list->argv[i],"<")==0)
                        {        
                            process_list->argv[i]=NULL;
                            strcpy(input,process_list->argv[i+1]);
                            in=2;           
                        }               

                        if(strcmp(process_list->argv[i],">")==0)
                        {      
                            process_list->argv[i]=NULL;
                            strcpy(output,process_list->argv[i+1]);
                            out=2;
                        }      
                }

                //if '<' char was found in string inputted by user
                if(in)
                {   
                    // fdo is file-descriptor
                    if ((fd0 = open(input, O_RDONLY, 0)) < 0) {
                        perror("Couldn't open input file");
                        exit(0);
                    }           
                    // dup2() copies content of fdo in input of preceeding file
                    dup2(fd0, 0); // STDIN_FILENO here can be replaced by 0 
                    close(fd0); // necessary
                }

                //if '>' char was found in string inputted by user 
                if (out)
                {
                    if ((fd1 = creat(output , 0644)) < 0) {
                        perror("Couldn't open the output file");
                        exit(0);
                    }           

                    dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                    close(fd1);
                }

                if (background){

                    setpgid(0,0);
                }
                
                if (process_list->argv == NULL){
                    // TODO decide what to do
                    commands_is_null(buffer);
                }
                
                //check if the command is exit
                else if (strcmp(process_list->argv[0], exit_command) == 0){
                    exit_cmd(buffer, process_list->argv);
                }

                //check if the command is env
                else if (strcmp(process_list->argv[0], env_command) == 0){
                    print_env(buffer, process_list->argv, env);
                }

                //check if the command is cd
                else if(strcmp(process_list->argv[0], "cd") == 0){
                    exit(EXIT_SUCCESS);
                }

                //check if the command is fg
                else if(strcmp(process_list->argv[0], "fg") == 0){
                    int job_num = check_job_number(process_list->argv[1]);
                    if (job_num == -1){
                        printf("no such job: %s", process_list->argv[1]);
                        exit(EXIT_FAILURE);
                    }
                    continue_job(&stopped_jobs, job_num);
                    exit(EXIT_SUCCESS);
                }

                //check if the command is bg
                else if(strcmp(process_list->argv[0], "bg") == 0){
                    int job_num = check_job_number(process_list->argv[1]);
                    if (job_num == -1){
                        printf("no such job: %s", process_list->argv[1]);
                        exit(EXIT_FAILURE);
                    }
                    continue_job(&stopped_jobs, job_num);
                    exit(EXIT_SUCCESS);
                }

                //check if the command is jobs
                else if(strcmp(process_list->argv[0], "jobs") == 0){
                    int job_num = check_job_number(process_list->argv[1]);
                    if (job_num == -1){
                        printf("no such job: %s\n", process_list->argv[1]);
                        exit(EXIT_FAILURE);
                    }
                    print_jobs(stopped_jobs, job_num);
                    exit(EXIT_SUCCESS);
                }

                //check if the command is a full path to command
                else if (stat(process_list->argv[0], &check_file) == 0){
                    if (execvp(process_list->argv[0], process_list->argv) < 0){
                        perror("error in execvp\n");
                        exit(EXIT_FAILURE);
                    }
                }

                //need to search the command in every directory from PATH and if is there run it
                else{
                    search_in_path(process_list->argv, env);
                }
                
            }

            //father process
            else{

                // close relevant pipes
                if (numPipes > 0){
                    if (j == 0){
                        close(filedes2[1]);
                    }
                    else if (process_list->next == NULL){
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
                    // only if this is the first process of the list (even if is just one command (e.g ls &), add to the list of jobs)
                    if (j==0){
                        add_job_to_list(&stopped_jobs, RUNNING);
                    }
                    
                }
                
                if (process_list->argv == NULL){
                    free(buffer);
                    free_duble_ptr(process_list->argv);
                }

                //check if the command is exit
                else if (strcmp(process_list->argv[0], exit_command) == 0){
                    exit_cmd(buffer, process_list->argv);
                }

                //check if the command is cd
                else if(strcmp(process_list->argv[0], "cd") == 0){
                    if (chdir(process_list->argv[1])!=0){
                        printf("could not cd into '%s'\n", process_list->argv[1]);
                    }
                }

                //check if the command is fg
                else if(strcmp(process_list->argv[0], "fg") == 0){
                    int job_num = check_job_number(process_list->argv[1]);
                    if (job_num == -1){
                        printf("wrong job id: %s", process_list->argv[1]);
                        exit(EXIT_FAILURE);
                    }
                     
                    remove_job_from_list(&stopped_jobs, job_num);
                    pid = waitpid(-1, &status, WUNTRACED);
                }

                //check if the command is fg
                else if(strcmp(process_list->argv[0], "bg") == 0){
                    int job_num = check_job_number(process_list->argv[1]);
                    if (job_num == -1){
                        printf("wrong job id: %s", process_list->argv[1]);
                        exit(EXIT_FAILURE);
                    }
                    remove_job_from_list(&stopped_jobs, job_num);
                }

                else{
                    free(buffer);
                    free_duble_ptr(process_list->argv);
                }

            }            
            process *temp = process_list;
            free(temp);
            temp = NULL;
            process_list = process_list->next;            
            j++;
        }
        buffer = NULL;
        background = 0;
        free(buff);
        buff = NULL;
        free_processes(process_list);
        process_list = NULL;
        print_shell(dolar);

    }

    return 0;
}