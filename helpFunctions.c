#include "funcs.h"

/* Built in exit command */
void exit_cmd(char **argv){
    free_duble_ptr(argv);
    save_history_to_file();
    exit(EXIT_SUCCESS);
}

/* Built in cd command */
void cd_cmd(char **argv){
    if (argv[1] == NULL)
        fprintf(stderr, "Please specify the directory\n");

    else {
        if (chdir(argv[1]) > 0)
            perror("cd error: ");
    }
}

void env_cmd(char **argv){
    int i=0;
    while (argv[i]){
        printf("%s\n", argv[i]);
        i++;
    }
}
void jobs_cmd(char **argv){
    (void)argv;
    print_jobs();
    return;
}

void commands_is_null(char *buffer){
    printf("command is null\n");
    free(buffer);
    buffer = NULL;
}

void free_processes(process* head){
    process* tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free_duble_ptr(tmp->argv);
        tmp->argv = NULL;
        free(tmp);
        tmp = NULL;
    }
    head = NULL;
}

void free_duble_ptr(char **ptr){
    int i=0;
    while (ptr[i]){
        free(ptr[i]);
        ptr[i] = NULL;
        i++;
    }
    free(ptr);
    ptr = NULL;
}

void print_shell(const char *dolar){
    char cwd[MAX_SIZE-7-3];
    char hostname[MAX_SIZE];

    char blue[MAX_SIZE] = "\x1B[34m";
    char green[MAX_SIZE] = "\x1B[92m";
    char white[MAX_SIZE] = "\x1B[37m";

    cwd[MAX_SIZE-1] = '\0';
    hostname[MAX_SIZE-1] = '\0';
    getcwd(cwd, MAX_SIZE-1);
    gethostname(hostname, MAX_SIZE-1);

    strcat(blue, cwd);
    strcat(blue, "\033");

    strcat(green, hostname);
    strcat(green, "\033");

    strcat(white, dolar);
    strcat(white, "\033");
    
    write(STDOUT_FILENO, green, strlen(hostname)+strlen("\033")+strlen("\x1B[93m")-1);
    write(STDOUT_FILENO, ":", 1);
    write(STDOUT_FILENO, blue, strlen(cwd)+strlen("\033")+strlen("\x1B[34m")-1);
    write(STDOUT_FILENO, white, strlen(white)-1);
}

int _strlen(char *buffer){
    int i=0;
    while(buffer[i] != '\0'){
        ++i;
    }
    return i;
}

int is_number(char *str){
    size_t i=0;
    for (; i < strlen(str); i++){
        if( !isdigit(str[i]))
            return 0;
    }
    return 1;
}