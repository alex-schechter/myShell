#include "funcs.h"

int main(int argc, char **argv, char **env){

    char **commands;
    char *buffer;
    char *env_command="env", *exit="exit", *dolar="$ ";
    ssize_t characters; size_t length;

    int pid;

    write(STDOUT_FILENO, dolar, 2);

    while((characters = getline(&buffer, &length, stdin))){
        printf("*******");
        printf("You typed: '%s'\n",buffer);
        
    }

    return 0;
}