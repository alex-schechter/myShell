#include "funcs.h"

extern char* buff;
extern pid_t pid;
extern pid_t shell_pid;


job *find_job_by_id(job *job_list, char *job_id){
    int id;
    job *curr = job_list;

    if (!(is_number(job_id))){
        return NULL;
    }
    else{
        id = atoi(job_id);
        while (curr!= NULL){
            if (curr->job_num == id){
                return curr;
            }
            curr = curr->next;
        }
        return NULL;
    }
}

job *get_last_job(job *job_list){
    job *curr = job_list;
    while (curr->next != NULL){
        curr = curr->next;
    }
    return curr;
}

void free_jobs(job *job_list){
    job *curr = job_list;
    job *temp;
    while (curr != NULL){
        temp = curr->next;
        free(curr->status);
        free(curr->command);
        curr->next=NULL;
        free(curr);
        curr = temp;
    }
}

void print_no_such_job(char *job_id){
    char message[MAX_SIZE];
    message[MAX_SIZE-1] = '\0';
    snprintf(message, MAX_SIZE, "no such job: %s\n", job_id);
    write(STDOUT_FILENO, message, strlen(message)+1);
}

void print_job(job *j){
    char message[MAX_SIZE];
    message[MAX_SIZE-1] = '\0';
    snprintf(message, MAX_SIZE, "[%d]+ %s %s (%d)\n", j->job_num, j->status, j->command, j->pid);
    write(STDOUT_FILENO, message, strlen(message)+1);
}

void print_jobs(job *job_list, char *job_id){
    job *curr = job_list;
    job *j = NULL;

    if (job_list == NULL){
            printf("there are no jobs\n");
            return;
    }

    if (job_id == NULL){
        while (curr != NULL){

            print_job(curr);
            curr = curr->next;
        }
        return;
    }
    
    j = find_job_by_id(job_list, job_id);
    if (j != NULL){
        print_job(j);
    }
    else{
        print_no_such_job(job_id);
    }
}

int get_list_length(job *list){
    int count = 0;
    job *curr = list;
    
    while (curr != NULL){
        count +=1;
        curr = curr->next;
    }
    return count;
}

void add_job_to_list(job **job_list,char *status){
    job *last_job;

    if(buff == NULL){
        return;
    }

    if ((*job_list) == NULL){
        (*job_list) = malloc(sizeof(job));
        if (!(*job_list)){
            perror("could not allocate memory");
        }
        last_job = *job_list;
        last_job->job_num = 1;
    }

    else{
        last_job = get_last_job(*job_list);
        last_job->next = malloc(sizeof(job));
        if (!last_job->next){
            perror("could not allocate memory");
            free_jobs(*job_list);
        }
        last_job->next->job_num = last_job->job_num + 1;
        last_job = last_job->next;
        
    }

    last_job->next = NULL;
    last_job->pid = pid;
    last_job->status = strdup(status);
    last_job->command = strdup(buff);
    if (!last_job->status || !last_job->command){
        perror("could not strdup\n");
        exit(EXIT_FAILURE);
    }
}


void remove_job_from_list(job **job_list, char *job_num){
    job *curr = *job_list;
    job *prev;
    int num;

    if (job_num == NULL){
        num = get_last_job(*job_list)->job_num;
    }
    else{
        if (!(is_number(job_num))){
            return;
        }
        num = atoi(job_num);
    }

    if (curr != NULL && curr->job_num == num) 
    { 
        *job_list = curr->next;
        free(curr);
        return; 
    }

    while (curr != NULL && curr->job_num != num) 
    { 
        prev = curr; 
        curr = curr->next; 
    } 
  
    if (curr == NULL){
        print_no_such_job(job_num);
        return;
    }

    prev->next = curr->next; 
    free(curr);
}

void continue_job(job **job_list, char *job_id){
    job *job_to_remove;
    int pid_to_cont = -1;

    if (!(*job_list)){
        write(STDOUT_FILENO, "There is not job to be countinued\n", strlen("There is not job to be countinued\n")+1);
        return;
    }
    
    if (job_id == NULL){        
        job_to_remove = get_last_job(*job_list);
        pid_to_cont = job_to_remove->pid;
        // printf("forgrounding process with pid: %d\n", pid_to_cont);
    }

    else{
        job_to_remove = find_job_by_id(*job_list, job_id);
        pid_to_cont = job_to_remove->pid;
        // printf("forgrounding process with pid: %d\n", pid_to_cont);
    }
    
    kill (pid_to_cont, SIGCONT);
}