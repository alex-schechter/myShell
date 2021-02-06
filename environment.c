#include "funcs.h"

void print_env(char *buffer, char **commands, char **env){
    int i=0;
    while (env[i]){
        printf("%s\n", env[i]);
        i++;
    }
    free(buffer);
    free_duble_ptr(commands);
    buffer = NULL;
    commands = NULL;
    exit (EXIT_SUCCESS);
}

int is_env_var(char *look_into, char *look_for){
    int i=0;
    while(look_for[i] && look_into[i]){
        if (look_for[i] != look_into[i])
        {
            return -1;
        }
        i++;
    }

    if (look_into[i] != '='){
        return -1;
    }
        
    return 0;
}

char *get_env_variable(char **env, char *env_var_2_look_for){
    int i=0;
    while (env[i]){
        if (is_env_var(env[i], env_var_2_look_for) == 0){
            return env[i];
        }
        i++;
    }
    return NULL;
}