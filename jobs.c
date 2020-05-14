#include "funcs.h"

// TODO figure out how to get the correct command (job_list->jobs[last_index].command)
void add_job_to_list(jobList *job_list,char *status){
    if (job_list->jobs == NULL){
        job_list->jobs = malloc(sizeof(job));
        if (job_list->jobs == NULL){
            free(job_list->jobs);
            perror("could not allocate memory");
        }
        job_list->size = 1;
    }
    else{
        job_list->jobs = (job *)realloc(job_list->jobs, sizeof(job) * (job_list->size + 1));
        if (job_list->jobs == NULL){
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