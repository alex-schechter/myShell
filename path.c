#include "funcs.h"

int get_path_length(char *path){
    int count=0, i=0;
    while (path[i]){
        if (path[i] == ':')
            count++;
        i++;
    }
    return count+1;
}

char **split_path(char *path, char *command){
    int path_vars_len;
    char **paths;
    char *token;
    int i=0;
    path_vars_len = get_path_length(path);
    /* +1 for null terminator at the end */
    paths = malloc(sizeof(char*) * (path_vars_len + 1));
    if (paths == NULL){
        return NULL;
    }
    path += 5;
    token = strtok(path, ":");
    while(token){
        if (token == NULL){
            return NULL;
        }
        /* The +2 is for '/' after the path and for the NULL terminator */
        paths[i] = (char *)malloc(sizeof(char) * (strlen(token) + 2 + strlen(command)));
        strncpy(paths[i], token, strlen(token));
        strcat(paths[i], "/");
        strcat(paths[i], command);
        token = strtok(NULL, ":");
        ++i;
    }

    paths[path_vars_len] = '\0';
    return paths;
}