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