#include "funcs.h"

extern int history_count;
extern int original_history_count;
extern history* first_history_command;

const char *const history_filename = "./.history.txt";

/* Get the last history command */
history *get_last_history(){
    history *curr = first_history_command;
    while (curr->next != NULL){
        curr = curr->next;
    }
    return curr;
}

/* Load the history file to linked list at startup */
void load_history_to_list(history **list) {
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    history* curr;
    int first = 1;

    file = fopen(history_filename, "a+" );
    if (file == NULL) {
        perror ("~/.history.txt"); 
        exit(EXIT_FAILURE);
    }

    /* Read line by line the history file */
    while ((read = getline(&line, &len, file)) != -1 ) {
        /* This is the first command that is being inserted to the list */
        if (*list == NULL) {
            *list = (history*)malloc(sizeof(history));
            (*list)->command = strdup(line);
            (*list)->next = NULL;
            
        }
        else {
            if (first == 1)
                curr = *list;
            curr->next = (history*)malloc(sizeof(history));
            curr = curr->next;
            curr->command = strdup(line);
            curr->next = NULL;
            first = 0;
        }
        ++history_count;
        ++original_history_count;
    }
}

/* Add history command to the list */
void write_to_history(char *command) {
    /* If the history file is empty */
    if (first_history_command == NULL) {
        first_history_command = (history*)malloc(sizeof(history));
        first_history_command->command = strdup(command);
        first_history_command->next = NULL;
    }
    else {
        history *last = get_last_history();
        last->next = (history*)malloc(sizeof(history));
        last = last->next;
        last->command = strdup(command);
        last->next = NULL;
    }
    ++history_count;
}

/* Print the last n history commands */
void print_last_n_commands(int number_of_commands) {
    int i = 0;
    history *curr = first_history_command;
    if (number_of_commands < 0) {
        fprintf(stderr, "history command should get positive number\n");
        return;
    }
    while (i <  history_count - number_of_commands) {
        curr = curr->next;
        ++i;
    }
    while (i < history_count) {
        printf("%d %s", i+1, curr->command);
        curr = curr->next;
        ++i;
    }
}

/* Save the new history commands to the history file file */
void save_history_to_file() {
    int i = 0;
    FILE *file;
    history *curr = first_history_command;

    /* Skip the commands that were already in the file */
    while (i < original_history_count) {
        curr = curr->next;
        ++i;
    }

    file = fopen(history_filename, "a" );
    if (file == NULL) {
        perror ("~/.history.txt"); 
        exit(EXIT_FAILURE);
    }

    while ( i < history_count) {
        fprintf(file, "%s", curr->command);
        curr = curr->next;
        ++i;
    }
    fclose(file);
}



// void history(char *command)
// {
//     char *token = command;
//     token = strtok(NULL, " \"\n\t\r"); 

//     int n = 10;
//     if(token != NULL)
//         n = atoi(token);

//     int count = n;
    
//     for(int i = hist_i - count; count > 0; i++, count--)
//     {
//         printf("%s", hist[i % 20]);
//         //count++;
//     }
    
// }