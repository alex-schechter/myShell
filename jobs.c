#include "funcs.h"


job *find_job_by_id(job *job_list, char *job_id){
    int id;
    job *curr = job_list;

    if (!(is_number(job_id))){
        return NULL;
    }
    else{
        id = atoi(job_id);
        while (curr->next != NULL){
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
    snprintf(message, MAX_SIZE, "[%d]+ %s %s\n", j->job_num, j->status, j->command);
    write(STDOUT_FILENO, message, strlen(message)+1);
}

void print_jobs(job *job_list, char *job_id){
    // write(STDOUT_FILENO, "**************\n", strlen("**************\n")+1);
    // print_job(job_list);
    // write(STDOUT_FILENO, "**************\n", strlen("**************\n")+1);
    job *curr = job_list;
    job *j = NULL;

    if (job_list == NULL){
            exit(EXIT_SUCCESS);
    }

    if (job_id == NULL){
        while (curr != NULL){
            print_job(curr);
            curr = curr->next;
        }
        exit(EXIT_SUCCESS);
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
    int count =0;
    job *curr = list;
    
    while (curr != NULL){
        count +=1;
        curr = curr->next;
    }
    return count;
}

// TODO figure out how to get the correct command (job_list->jobs[last_index].command)
void add_job_to_list(job **job_list,char *status){
    job *last_job;
    if ((*job_list) == NULL){
        write(STDOUT_FILENO, "**************\n", strlen("**************\n")+1);
        (*job_list) = malloc(sizeof(job));
        if (!(*job_list)){
            perror("could not allocate memory");
        }
        last_job = *job_list;
        // (*job_list)->job_num = 1;
        // (*job_list)->pid = getpid();
        // (*job_list)->status = strdup(status);
        // (*job_list)->command = strdup("asd");
        // (*job_list)->next = NULL;
    }

    else{
        last_job = get_last_job(*job_list);
        last_job->next = malloc(sizeof(job));
        if (!last_job->next){
            perror("could not allocate memory");
            free_jobs(*job_list);
        }
        last_job = last_job->next;
    }
        last_job->job_num = get_list_length(*job_list);
        last_job->pid = getpid();
        last_job->status = strdup(status);
        last_job->command = strdup("asd");
        last_job->next = NULL;
}


void remove_job_from_list(job **job_list, int index_to_delete, int *size){
    // int i=index_to_delete;
    // for (;i<*size-1; i++){
    //     (*job_list)[i] = (*job_list)[i+1];
    // }
    
    // (*job_list) = (job *)realloc((*job_list), sizeof(job) * (*size - 1));
    // *size -= 1;
    // // return job_list;
}

void make_forground(job **job_list, char *job_id){
    
    // job *last_job;
    // if (job_id == NULL){        
    //     remove_job_from_list(&job_list->jobs, job_list->size - 1, &job_list->size);
    //     printf("size is %d\n", job_list->size);
    // }

    // else{
    // }
    
    // find_job_by_id(job_list, job_id);
}