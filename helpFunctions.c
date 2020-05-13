#include "funcs.h"


void commands_is_null(char *buffer){
    printf("command is null\n");
    free(buffer);
}

void exit_cmd(char *buffer, char **commands){
    free(buffer);
    free_duble_ptr(commands);
    exit(EXIT_SUCCESS);
}

int _strlen(char *buffer){
    int i=0, counter=0;
    while(buffer[i] != '\0'){
        ++counter;
        ++i;
    }
    return counter;
}

void free_duble_ptr(char **ptr){
    int i=0;
    while (ptr[i]){
        free(ptr[i]);
        i++;
    }
    free(ptr);
}

void print_shell(char *dolar){
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