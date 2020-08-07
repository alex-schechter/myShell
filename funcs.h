#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_SIZE 1024

#define RUNNING "Running"
#define STOPPED "Stopped"
#define DONE "Done"

//every process is a commend that is splitted by pipe (|) or a regular command  (e.g ls)
typedef struct process{
    char **argv;
    struct process *next;
    pid_t pid;
    int stopped;
}process;

// job is a group of processes
typedef struct job{
    char *status;
    char *command;
    int job_num;
    pid_t pgid;
    process *first_process;
    struct job *next;
}job;

//jobs
job *find_job_by_id(job *job_list, int job_id);
job *find_job_by_pid(job *job_list, int pid);
job *get_last_job(job *job_list);
void free_jobs(job *job_list);
void print_job(job *j);
void print_jobs(job *jobs, int job_id);
int get_list_length(job *list);
void add_job_to_list(job **job_list,char *status);
void remove_job_from_list(job **job_list, int job_num);
void continue_job(job **job_list, int job_id);
int check_job_number(char *job_number);


//commands
int get_number_of_pipes(char *buffer);
process *split_by_pipe(char *buffer);
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
void free_processes(process* head);
void free_duble_ptr(char **ptr);
void print_shell(char *);
int is_number(char *);


//hendlers
void INThandler(int);
void TSTPhandler(int);
void CONThandler(int);
void CHLDhandler(int);