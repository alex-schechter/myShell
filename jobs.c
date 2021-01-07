#include "funcs.h"

extern char* buff;
extern pid_t pid;
extern pid_t shell_pid;
extern int shell_is_interactive;
extern int job_count;
extern job* first_job;

char *built_in_commands[] = {
    "exit",
    "env",
    "cd",
    "jobs"
};

int (*built_in_funcs[])(char **argv) = {
    &exit_cmd,
    &env_cmd,
    &cd_cmd,
    &jobs_cmd
};

int built_in_funcs_count() {
    return sizeof(built_in_commands) / sizeof(char *);
}

// job *find_job_by_id(job *job_list, int job_id){
//     while (curr!= NULL){
//         if (curr->job_num == job_id){
//             return curr;
//         }
//         curr = curr->next;
//     }
//     return NULL;
// }


/* Find job by pgid */
job *find_job_by_pgid(int pgid){
    for (job *j = first_job; j; j = j->next){
        if (j -> pgid == pgid)
            return j;
    }
    return NULL;
}

/* Find prev job by pgid */
job *find_prev_job_by_pgid(int pgid){
    for (job *j = first_job; j -> next ; j = j->next){
        if (j -> next -> pgid == pgid)
            return j;
    }
    return NULL;
}

job *get_last_job(job *job_list){
    job *curr = job_list;
    while (curr->next != NULL){
        curr = curr->next;
    }
    return curr;
}

// void free_jobs(job *job_list){
//     job *curr = job_list;
//     job *temp;
//     while (curr != NULL){
//         temp = curr->next;
//         free(curr->status);
//         free(curr->command);
//         curr->next=NULL;
//         free(curr);
//         curr = temp;
//     }
// }

// void print_no_such_job(int job_id){
//     printf("no such job: %d\n", job_id);
// }

void print_job(job *j, char *sign, int place_in_list){
    char *status = NULL;
    if (job_is_finished(j))
        status = "DONE";
    else if (job_is_stopped(j))
        status = "Stopped";
    else 
        status = "Running";

    printf("[%d]%s  %s                 %s\n", place_in_list, sign, status, j->command);
}

void print_jobs(){
    job *curr = first_job;
    job *j = NULL;
    char *sign = " ";
    int i = 1;

    if (first_job == NULL){
            printf("there are no jobs\n");
            return;
    }

    while (curr->next != NULL){
        /* If this is the last job */
        if (i+1 == job_count){
            sign = "+";
        }
        else if (i+2 == job_count)
        {
            sign = "-";
        }
        
        print_job(curr, sign, i);
        curr = curr->next;
        ++i;
    }
    return;
    
    
    // j = find_job_by_id(job_list, job_id);
    // if (j != NULL){
    //     print_job(j);
    // }
    // else{
    //     print_no_such_job(job_id);
    // }
}

// int get_list_length(job *list){
//     int count = 0;
//     job *curr = list;
    
//     while (curr != NULL){
//         count +=1;
//         curr = curr->next;
//     }
//     return count;
// }

// void add_job_to_list(job **job_list,char *status){
//     job *last_job;

//     if(buff == NULL){
//         return;
//     }

//     if ((*job_list) == NULL){
//         (*job_list) = malloc(sizeof(job));
//         if (!(*job_list)){
//             perror("could not allocate memory");
//         }
//         last_job = *job_list;
//         last_job->job_num = 1;
//     }

//     else{
//         last_job = get_last_job(*job_list);
//         last_job->next = malloc(sizeof(job));
//         if (!last_job->next){
//             perror("could not allocate memory");
//             free_jobs(*job_list);
//         }
//         last_job->next->job_num = last_job->job_num + 1;
//         last_job = last_job->next;
//     }

//     last_job->next = NULL;
//     last_job->pgid = pid;
//     last_job->status = strdup(status);
//     last_job->command = strdup(buff);
    
//     if (!last_job->status || !last_job->command){
//         perror("could not strdup\n");
//         exit(EXIT_FAILURE);
//     }

//     printf("[%d]+ %s %s\n", last_job->job_num, last_job->status, last_job->command);
// }


// void remove_job_from_list(job **job_list, int job_num){
//     job *curr = *job_list;
//     job *prev;
//     int num;

//     if (job_num == 0){
//         num = get_last_job(*job_list)->job_num;
//     }
//     else{
//         num = job_num;
//     }

//     if (curr != NULL && curr->job_num == num) 
//     { 
//         *job_list = curr->next;
//         free(curr);
//         return; 
//     }

//     while (curr != NULL && curr->job_num != num) 
//     { 
//         prev = curr; 
//         curr = curr->next; 
//     } 
  
//     if (curr == NULL){
//         print_no_such_job(job_num);
//         return;
//     }

//     prev->next = curr->next; 
//     free(curr);
// }

// void continue_job(job **job_list, int job_id){
//     job *job_to_remove;
//     int pid_to_cont = -1;

//     if (!(*job_list)){
//         printf("There is no job to be countinued\n");
//         return;
//     }
    
//     if (job_id == 0){        
//         job_to_remove = get_last_job(*job_list);
//         pid_to_cont = job_to_remove->pgid;
//         // printf("forgrounding process with pid: %d\n", pid_to_cont);
//     }

//     else{
//         job_to_remove = find_job_by_id(*job_list, job_id);
//         pid_to_cont = job_to_remove->pgid;
//         // printf("forgrounding process with pid: %d\n", pid_to_cont);
//     }
    
//     kill (pid_to_cont, SIGCONT);
// }


int check_job_number(char *job_number){
    int job_num;
    if (job_number == NULL){
        return 0;
    }
    else{
        job_num = atoi(job_number);
        if (job_num == 0){
            return -1;
        }
        return job_num;
    }
}


void launch_job (job *j, int foreground, char **env) {
    pid_t pid;
    int mypipe[2], infile, outfile;
    int first_process=1;

    infile = j->stdin;

    /* Iterate over the processes of the job */
    for (process *p = j->first_process; p; p = p->next)
    {
        int i = 0;
        int job_number = 0;
        // int built_in_command = 0;
        /* Set up pipes, if necessary */
        if (p->next)
        {
            if (pipe (mypipe) < 0)
            {
                perror ("pipe");
                exit (1);
            }
            outfile = mypipe[1];
        }
        else
            outfile = j->stdout;

        /* Check for built in functions ( exit, env, cd, jobs ) */
        for (int i = 0; i<built_in_funcs_count(); i++){
            if (strcmp(p->argv[0], built_in_commands[i]) == 0) {
                if (strcmp(p->argv[0], "env") == 0)
                    (*built_in_funcs[i])(env);
                else
                    (*built_in_funcs[i])(p->argv);
                p->finished = 1;
                goto clean_pipes;
            }
        }

        /* If the command is fg or bg*/
        if (strcmp(p->argv[0], "fg") == 0 || strcmp(p->argv[0], "bg") == 0) {
            if (strcmp(p->argv[0], "fg") == 0)
                foreground = 1;
            else
                foreground = 0;

            /* Should continue the first job */
            if (p->argv[1] == NULL) {
                job_number = job_count - 1;
            }
            else {
                job_number = atoi(p->argv[1]);
            }
            job *nth_job = get_the_nth_job(job_number);
            if (nth_job == NULL) {
                    fprintf(stderr, "Job number %s does not exist\n", p->argv[1]);
                    p->finished = 1;
                    return;
            }
            continue_job(nth_job, foreground);
            p->finished = 1;
            goto clean_pipes;
            
        }

        /* Fork the child processes */
        pid = fork ();

        /* The fork failed */
        if (pid < 0)
        {
            perror ("fork");
            exit (1);
        }

        /* This is the child process */
        else if (pid == 0) {
            /* If this is the first process in the process group set 
                the pgid to be the pid of the first process */
            if (first_process == 1) {
                j->pgid = pid;
            }
            launch_process (p, j->pgid, infile, outfile, j->stderr, foreground);
        }   
            

        /* This is the parent process */
        else
        {
            p->pid = pid;
            if (shell_is_interactive)
            {
                if (!j->pgid)
                    j->pgid = pid;
                setpgid (pid, j->pgid);
            }
        }

        first_process = 0;

    clean_pipes:
        /* Clean up after pipes */
        if (infile != j->stdin)
            close (infile);
        if (outfile != j->stdout)
            close (outfile);
        infile = mypipe[0];
    }

    format_job_info (j, "launched");

    if (!shell_is_interactive)
        wait_for_job (j);
    else if (foreground)
        put_job_in_foreground (j, 0);
    else
        put_job_in_background (j, 0);
}


/* Check for processes that have status information available,
   blocking until all processes in the given job have reported */
void wait_for_job (job *j) {
    // printf("started waiting for job: %s", j->command);
    // printf("job_is_finished: %d\n", job_is_finished (j));
    int status;
    pid_t pid;

    if (job_is_finished(j))
        return;
    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status (pid, status)
            && !job_is_stopped (j)
            && !job_is_finished (j));
}

/* Format information about job status for the user to look at */
void format_job_info (job *j, const char *status) {
    fprintf (stderr, "%ld (%s): %s", (long)j->pgid, status, j->command);
}

/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list */
void do_job_notification() {
    job *j, *jlast, *jnext;

    /* Update status information for child processes */
    update_status();

    jlast = NULL;

    // printf("first_job command is: %s\n", first_job->command);
    for (j = first_job; j; j = jnext)
    {
        // printf("working on job with command %s at address %p\n", j->command, j);
        jnext = j->next;

        /* If all processes have completed, tell the user the job has
            completed and delete it from the list of active jobs */
        if (job_is_finished (j)) {
            format_job_info (j, "completed");
            job_count -= 1;
            
            if (jlast) {
                jlast->next = jnext;
            }
            else {
                first_job = jnext;
            }

            // TODO think about it
            // free_job(j);
            // j = NULL;

        }

        /* Notify the user about stopped jobs,
            marking them so that we won’t do this more than once */
        else if (job_is_stopped (j) && !j->notified) {
            format_job_info (j, "stopped");
            j->notified = 1;
            jlast = j;
        }

        /* Don’t say anything about jobs that are still running */
        else
            jlast = j;
    }
}


/* Mark a stopped job j as being running again */
void mark_job_as_running (job *j) {
    for (process *p = j->first_process; p; p = p->next)
        p->stopped = 0;
    j->notified = 0;
}

/* Continue the job */
void continue_job (job *j, int foreground) {
    mark_job_as_running (j);
    if (foreground)
        put_job_in_foreground (j, 1);
    else
        put_job_in_background (j, 1);
}

/* Free the job and remove it from the list of active jobs */
void free_job(job *j) {
    printf("im here\n");
    if (j==NULL)
        return;
    printf("the j data is: %p\n", j);
    printf("im 2222 here\n");
    
    char *c = strdup(j->command);
    printf("im 2222 here\n");

    printf("freeing job with command %s\n", j->command);
    free(j->command);
    j->command = NULL;
    free_processes(j->first_process);
    j->first_process = NULL;
    free(j);
    j = NULL;
    job_count -= 1;
    // printf("Done freeing job with command %s, the job list is: \n", c);
    print_jobs();
}

/* Creates job from command inputed by the user */
job *create_job_from_command(char *command) {
    job *new_job = NULL;
    process *process_list = NULL;

    /* Check if the command inputed by the user is empty */
    if (strncmp(command, "\n", strlen(command)) == 0) {
        command = NULL;
        return NULL;
    }

    /* Split the command by pipes to create a process list */
    process_list = split_by_pipe(command);

    if (process_list == NULL) {
        free(command);
        command = NULL;
        return NULL;
    }
    
    new_job = (job *)malloc(sizeof(job));
    if (new_job == NULL) {
        perror("Couldn't create a new job");
        free(command);
        command = NULL;
        return NULL;
    }

    new_job->first_process = process_list;
    new_job->command = strdup(command);
    new_job->notified = 0;
    new_job->pgid = 0;
    new_job->stdin = STDIN_FILENO;
    new_job->stdout = STDOUT_FILENO;
    new_job->stderr = STDERR_FILENO;

    return new_job;
}

/* Check if job is stopped */
int job_is_stopped(job *j) {
    for (process *p = j->first_process; p; p=p->next) {
        if (!p->finished && !p->stopped)
            return 0;
    }
    return 1;
}

/* Check if job is finished ( i.e all of its processes are finished) */
int job_is_finished (job *j) {
    for (process *p = j->first_process; p; p = p->next) {
        if (!p->finished)
            return 0;
    }
    return 1;
}

/* Gets the Nth job in the list */
job *get_the_nth_job(int job_number) {
    // printf("the job number is %s\n", job_number);
    job *current = first_job;
    int index = 1;
    
    while (current != NULL) {
        if (job_number == index) {
            if (current->next == NULL) 
                return NULL;
            return current;
        }
        index++;
        current = current->next;
    }
    return NULL;
}