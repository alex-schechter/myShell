#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <signal.h>
#include <ctype.h>

#define MAX_SIZE 1024

#define RUNNING "RUNNING"
#define STOPPED "STOPPED"
#define DONE "DONE"

typedef struct job{
    int job_num;
    int pid;
    char *status;
    char *command;
    struct job *next;
}job;

//jobs
job *find_job_by_id(job *job_list, char *job_id);
job *get_last_job(job *job_list);
void free_jobs(job *job_list);
void print_job(job *j);
void print_jobs(job *jobs, char *job_id);
int get_list_length(job *list);
void add_job_to_list(job **job_list,char *status);
void remove_job_from_list(job **job_list, int index_to_delete, int *size);
void make_forground(job **job_list, char *job_id);


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
void print_shell(char *);
int is_number(char *);


//hendlers
void INThandler(int);
void TSTPhandler(int);
void CONThandler(int);