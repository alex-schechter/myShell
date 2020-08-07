#include "funcs.h"

extern char *buff;

int get_commands_length(char *buffer){
    int count = 0, i = 0, is_candidate = 0;

    while (buffer[i]!= '\0'){
        if (buffer[i] != ' '){
            is_candidate = 1;
        }

        if ((is_candidate==1 && buffer[i + 1] == ' ') || (is_candidate==1 && buffer[i + 1] == '\0'))
        {
            ++count;
            is_candidate = 0;
        }   
        i++;
    }
    
    return count;
}

int get_number_of_pipes(char *buffer){
    int count = 0, i = 0, is_candidate = 0;
    while (buffer[i] != '\0'){
        if (buffer[i] == '|'){
            is_candidate = 1;
        }

        if ((is_candidate==1 && buffer[i + 1] != '|') || (is_candidate==1 && buffer[i + 1] == '\0'))
        {
            ++count;
            is_candidate = 0;
        }   
        i++;
    }
    return count;
}

void slice_str(const char * str, char * buffer, int start, int end)
{
    int j = 0;
    int i;
    for (i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

process *split_by_pipe(char *buffer){
    process *head = malloc(sizeof(process));
    head->next = NULL;
    process *p = head;
    size_t start = 0;
    size_t end = 0;
    int candidate = 0;

    while (end != strlen(buffer) + 1){

        if (buffer[end] == '|' || buffer[end] == '\0'){
            char *str = malloc(end-start + 1);
            str[end-start] = '\0';
            slice_str(buffer, str, start, end-1);
            p->argv = parse_commands(strdup(str));
            if (p->argv == NULL){
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
            p->stopped = 0;

            if (strchr(str, '&')){
                candidate = 1;
            }

            if (candidate == 1 && buffer[end] == '|'){
                return NULL;
            }

            if (buffer[end] != '\0'){
                printf("creating new node for command\n");
                p->next = malloc(sizeof(process));
                if (p->next==NULL){
                    perror("Error allocating memory");
                    exit(EXIT_FAILURE);
                }
            }
            
            start = end+1;
            ++end;
            p = p->next;
        }

        else {
            ++end;
        }
    }
    return head;
}

char **parse_commands(char *buffer){
    char **commands = NULL;
    char *command;
    int i;
    int commands_length;
    
    buffer[_strlen(buffer) -1 ] = '\0';
    if (buff == NULL){
        perror("Error allocating  memory");
        exit(EXIT_FAILURE);
    }
    commands_length = get_commands_length(buffer);

    // the length +1 is for NULL terminator at the end
    commands = malloc(sizeof(char *) * (commands_length + 1));
    if (commands == NULL){
        perror("could not allocate memory");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    //split the commands by ' '
    command = strtok(buffer, " ");
    if (command == NULL){
        free(commands);
        return NULL;
    }

    i = 0;
    while (command != NULL){
        if (command == NULL){
            free_duble_ptr(commands);
            return NULL;
        }
        commands[i] = (char *)malloc(sizeof(char) * strlen(command) + 1);
        strcpy(commands[i], command);
        command = strtok(NULL, " ");
        ++i;
    }

    commands[commands_length] = '\0';

    return commands;
}

void search_in_path(char **commands, char **env){
    char **path_vars;
    char *path;
    struct stat check_file;

    // get the PATH env variable
    path = get_env_variable(env, "PATH");

    // split the path to all the paths concatenated to the command
    path_vars = split_path(path, commands[0]);
    if (path_vars == NULL){
        free_duble_ptr(commands);
        free_duble_ptr(path_vars);
        free(path);
        exit(EXIT_FAILURE);
    }

    int i=0;
    while (path_vars[i]){
        if (stat(path_vars[i], &check_file) == 0){
            execvp(path_vars[i], commands);
            exit(EXIT_SUCCESS);
        }
        i++;
    }
    printf("Command '%s' not found\n", commands[0]);
    free_duble_ptr(commands);
    free_duble_ptr(path_vars);
    exit(EXIT_FAILURE);
}