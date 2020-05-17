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
}job;

typedef struct jobList
{
    int size;
    job *jobs;
}jobList;

//jobs
void find_job_by_id(const jobList job_list, char *job_id);
void print_job(const job j);
void print_jobs(const jobList jobs, const char *job_id);
void add_job_to_list(jobList *job_list, char *status);
void remove_job_from_list(job **job_list);

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