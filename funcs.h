#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define MAX_SIZE 1024

#define RUNNING "Running"
#define STOPPED "Stopped"
#define DONE "Done"

/* Every process is a commend that is splitted by pipe (|) or a 
    regular command (e.g ls) */
typedef struct process{
    struct process *next;       /* next process in pipeline                     */
    char **argv;                /* arguments for exec                           */
    pid_t pid;                  /* process PID                                  */
    int stopped;                /* true if process is stopped                   */
    int finished;               /* true if process if completed                 */
    int status;                 /* reported status value                        */
} process;

/* Job is a group of processes */
typedef struct job{
    struct job *next;           /* next active job                              */
    char *command;              /* command line, used for messages              */
    process *first_process;     /* the first process of the list in this job    */
    pid_t pgid;                 /* process group ID                             */
    int notified;               /* true if user told about stopped job          */
    struct termios tmodes;      /* saved terminal modes                         */
    int stdin, stdout, stderr;  /* stardard i/o                                 */
} job;

/* History linked list */
typedef struct history{ 
    struct history *next;       /* next command from history                    */
    char *command;              /* history command                              */
} history;

/* Shell functions */
void init_shell();
void put_job_in_foreground (job *j, int cont);
void put_job_in_background (job *j, int cont);

/* Jobs functions */
job *find_job_by_id(job *job_list, int job_id);
job *find_job_by_pgid(int pgid);
job *find_prev_job_by_pgid(int pgid);
job *get_last_job(job *job_list);
void free_jobs(job *job_list);
void print_job(job *j, char *sign, int place_in_list);
void print_jobs();
void add_job_to_list(job **job_list,char *status);
/* void continue_job(job **job_list, int job_id); */
int get_list_length(job *list);
int check_job_number(char *job_number);


job *create_job_from_command(char *command);
job *get_the_nth_job(int job_number);
void launch_job(job *j, int foreground, char **env);
void wait_for_job (job *j);
void format_job_info (job *j, const char *status);
void do_job_notification ();
void mark_job_as_running (job *j);
void continue_job (job *j, int foreground);
void free_job(job *j);
int built_in_funcs_count();
int job_is_stopped(job *j);
int job_is_finished (job *j);

/* Process functions*/
void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground);
void update_status (void);
int mark_process_status (pid_t pid, int status);

/* History functions*/
history *get_last_history();
void load_history_to_list(history **list);
void write_to_history(char *command);
void print_last_n_commands(int number_of_commands);
void save_history_to_file();

/* Commands functions*/
process *split_by_pipe(char *buffer);
char **parse_commands(char *buffer);
void search_in_path(char **commands, char **env);
int get_number_of_pipes(char *buffer);
int get_commands_length(char *buffer);


/* Environment functions */
char *get_env_variable(char **env, char *env_var_2_look_for);
void print_env(char *buffer, char **commands, char **env);
int is_env_var(char *look_into, char *look_for);


/* Path functions */
char **split_path(char *path, char *command);
int get_path_length(char *path);


/* Help functions */
void exit_cmd(char **argv);
void env_cmd(char **argv);
void cd_cmd(char **argv);
void jobs_cmd(char **argv);
void fg_cmd(char **argv);
void bg_cmd(char **argv);
void commands_is_null(char *buffer);
void free_processes(process* head);
void free_duble_ptr(char **ptr);
void print_shell(const char *);
int _strlen(char *buffer);
int is_number(char *);


/* Signal hendlers */
void INThandler(int);
void TSTPhandler(int);
void CONThandler(int);
void CHLDhandler(int);