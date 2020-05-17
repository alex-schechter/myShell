#include "funcs.h"


void find_job_by_id(const jobList job_list, char *job_id){
    char message[MAX_SIZE];
    int i=0;
    int id, found = 0;

    message[MAX_SIZE-1] = '\0';
    
    if (!(is_number(job_id))){
        snprintf(message, MAX_SIZE, "no such job: %s\n", job_id);
        write(STDOUT_FILENO, message, strlen(message)+1);
    }
    else{
        id = atoi(job_id);
        for(; i<job_list.size; i++){
            if (job_list.jobs[i].job_num == id){
                print_job(job_list.jobs[i]);
                found = 1;
                break;
            }
        }
        if (!found){
            snprintf(message, MAX_SIZE, "no such job: %s\n", job_id);
            write(STDOUT_FILENO, message, strlen(message)+1);
        }
    }
}

void print_job(const job j){
    char message[MAX_SIZE];
    message[MAX_SIZE-1] = '\0';
    snprintf(message, MAX_SIZE, "[%d]+ %s %s\n", j.job_num, j.status, j.command);
    write(STDOUT_FILENO, message, strlen(message)+1);
}

void print_jobs(const jobList job_list, const char *job_id){
    char message[MAX_SIZE];
    int size = job_list.size;
    int i=0;
    message[MAX_SIZE-1] = '\0';
    if (job_id == NULL){
        for(; i<size; i++){
            print_job(job_list.jobs[i]);
        }
        exit(EXIT_SUCCESS);
    }
    
    find_job_by_id(job_list, job_id);
}



// TODO figure out how to get the correct command (job_list->jobs[last_index].command)
void add_job_to_list(jobList *job_list,char *status){
    if (job_list->jobs == NULL){
        job_list->jobs = malloc(sizeof(job));
        if (!job_list->jobs){
            free(job_list->jobs);
            perror("could not allocate memory");
        }
        job_list->size = 1;
    }
    else{
        job_list->jobs = (job *)realloc(job_list->jobs, sizeof(job) * (job_list->size + 1));
        if (!job_list->jobs){
            free(job_list->jobs);
            perror("could not allocate memory");
        }
        job_list->size += 1;
    }

    int last_index = job_list->size-1;
    job_list->jobs[last_index].job_num = last_index+1;
    job_list->jobs[last_index].pid = getpid();
    job_list->jobs[last_index].status = status;
    job_list->jobs[last_index].command = "asd";
}


void remove_job_from_list(job **job_list){

}