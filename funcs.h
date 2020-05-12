#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include  <signal.h>

//commands
char **parse_commands(char *buffer);
int get_commands_length(char *buffer);
void search_in_path(char **commands, char **env);


//environment
void print_env(char *buffer, char **commands, char **env);
char *get_env_variable(char **env, char *env_var_2_look_for);
int is_env_var(char *look_into, char *look_for);


//path
char **split_path(char *path, char *command);
int get_path_length(char *path);


//help functions
void commands_is_null(char *buffer);
void exit_cmd(char *buffer, char **commands);
int _strlen(char *buffer);
void free_duble_ptr(char **ptr);

//hendlers
void INThandler(int);
void CHILDhandler(int);