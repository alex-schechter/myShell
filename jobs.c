#include "funcs.h"

extern char* buff;
extern pid_t pid;
extern pid_t shell_pid;


job *find_job_by_id(job *job_list, int job_id){
    job *curr = job_list;

    while (curr!= NULL){
        if (curr->job_num == job_id){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

job *find_job_by_pid(job *job_list, int pid){
    job *curr = job_list;

    while (curr!= NULL){
        if (curr->pgid == pid){
            return curr;
        }
        curr = curr->next;
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

void print_no_such_job(int job_id){
    printf("no such job: %d\n", job_id);
}

void print_job(job *j){
    printf("[%d]+ %s %s (%d)\n", j->job_num, j->status, j->command, j->pgid);
}

void print_jobs(job *job_list, int job_id){
    job *curr = job_list;
    job *j = NULL;

    if (job_list == NULL){
            printf("there are no jobs\n");
            return;
    }

    if (job_id == 0){
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
    last_job->pgid = pid;
    last_job->status = strdup(status);
    last_job->command = strdup(buff);
    
    if (!last_job->status || !last_job->command){
        perror("could not strdup\n");
        exit(EXIT_FAILURE);
    }

    printf("[%d]+ %s %s\n", last_job->job_num, last_job->status, last_job->command);
}


void remove_job_from_list(job **job_list, int job_num){
    job *curr = *job_list;
    job *prev;
    int num;

    if (job_num == 0){
        num = get_last_job(*job_list)->job_num;
    }
    else{
        num = job_num;
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

void continue_job(job **job_list, int job_id){
    job *job_to_remove;
    int pid_to_cont = -1;

    if (!(*job_list)){
        printf("There is no job to be countinued\n");
        return;
    }
    
    if (job_id == 0){        
        job_to_remove = get_last_job(*job_list);
        pid_to_cont = job_to_remove->pgid;
        // printf("forgrounding process with pid: %d\n", pid_to_cont);
    }

    else{
        job_to_remove = find_job_by_id(*job_list, job_id);
        pid_to_cont = job_to_remove->pgid;
        // printf("forgrounding process with pid: %d\n", pid_to_cont);
    }
    
    kill (pid_to_cont, SIGCONT);
}


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