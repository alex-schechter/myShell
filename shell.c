#include "funcs.h"

extern pid_t shell_pgid;
extern int shell_terminal;
extern int shell_is_interactive;
extern struct termios shell_tmodes_old;
extern struct termios shell_tmodes_new;

/* Search in all the paths in $path variable for commands 
	that start with the prefix and then in the current dir */
command *find_commands_starts_with_prefix(char **env, char *prefix, int *size) {
	command *first_option = NULL, *current_option = NULL;
    char *path, *token;
	char filename_qfd[300];
	struct dirent *dp;
	int count = 0;
	DIR *dfd;

    /* get the $PATH variable */
    path = strdup(get_env_variable(env, "PATH"));

	token = strtok(path, ":");
	/* Skip the PATH= literal at the beginning */
	token += 5;
	/* Go over each path in the splited by : */
    while(token){
        if (token == NULL){
            return NULL;
        }

		/* Open the dir to scan the files in it */
		if ((dfd = opendir(token)) == NULL) {
			perror(token);
		}

		/* Iterate over each file in the dir */
		while ((dp = readdir(dfd)) != NULL) {
			struct stat stbuf;
			/* Put the full program path in filename_qfd*/
			sprintf( filename_qfd , "%s/%s",token, dp->d_name) ;
			if( stat(filename_qfd,&stbuf ) == -1 ) {
				continue;
			}
			/* Skip directories */
			if (( stbuf.st_mode & S_IFMT ) == S_IFDIR ) {
				continue;
			}
			else {
				/* If the program starts with the prefix */
				if (strncmp(dp->d_name, prefix, strlen(prefix)) == 0) {
					++count;
					/* If this is the first option */
					if (first_option == NULL) {
						first_option = malloc(sizeof(command *));
						if (first_option == NULL) {
							perror("malloc");
							exit(EXIT_FAILURE);
						}
						first_option->data = strdup(dp->d_name);
						first_option->next = NULL;
						current_option = first_option;
						++(*size);
					}
					else {
						current_option->next = malloc(sizeof(command *));
						if (current_option->next == NULL) {
							perror("malloc");
							exit(EXIT_FAILURE);
						}
						current_option->next->data = strdup(dp->d_name);
						current_option->next->next = NULL;
						current_option = current_option->next;
						++(*size);
					}
				}
			}
		}
		closedir(dfd);
		token = strtok(NULL, ":");
    }
	return first_option;
}

/* Handles the input loop */
char *handle_input(char **env) {
	char c;
	int len, tab_option = 0;
	int command_len, actual_buffer_len;
	char *buffer = NULL, *final_command = NULL;
	command *first_suggestion = NULL;


	while ((c = getchar())) {
		
		if (buffer == NULL) {
			buffer = (char *)malloc(sizeof(char *)*2);
			if (buffer == NULL) {
				perror("malloc: ");
				exit(EXIT_FAILURE);
			}
			command_len = 0;
			actual_buffer_len = 1;
		}

		else if (command_len + 1 >= actual_buffer_len) {
			buffer = (char *)realloc(buffer, command_len*2 + 1);
			if (buffer == NULL) {
				perror("realloc: ");
				exit(EXIT_FAILURE);
			}
			actual_buffer_len = actual_buffer_len * 2;
		}

		/* If the user hits enter */
		if (c == 10) {
			putchar(c);
			buffer[command_len] = '\0';
			++command_len;
			tab_option = 0;
			break;
		}

		/* Tab */ 
		else if (c == 9) {
			++tab_option;
			/* If I have pressed double tab and there 
				is no input yet ignore it */
			if (tab_option == 2){
				if (command_len == 0)
					continue;
				else {
					int size = 0;
					if (first_suggestion != NULL) {
						free_all_suggestions(first_suggestion);
						first_suggestion = NULL;
					}
					first_suggestion = find_commands_starts_with_prefix(env, buffer, &size);
					if (size > 20) {
						printf("\nDisplay all %d possibilities? (y or n) ", size);
						do {
							c = getchar();
						} while ( c != 121 && c != 110);
						putchar(c);
						/* If the user typed y*/
						if (c == 121) {
							print_all_suggestions(first_suggestion);
						}
						free_all_suggestions(first_suggestion);
						first_suggestion = NULL;
						fputs("\n", stdout);
						print_shell("$ ");
						fputs(buffer, stdout);
					}
					else if (size > 0) {
						print_all_suggestions(first_suggestion);
						free_all_suggestions(first_suggestion);
						first_suggestion = NULL;
						fputs("\n", stdout);
						print_shell("$ ");
						fputs(buffer, stdout);
					}
				}
				tab_option = 0;
			}
			else
				continue;
		}

		/* Ctrl-C */ 
		else if (c == 3) {
			printf("^C\n");
			return "";
		}

		/* End of input */
		else if (c == 4) {
			buffer = strdup("exit");
			command_len = strlen(buffer);
			break;
		}
		/* Backslash */
		else if (c == 127 || c == 8) {
			if (command_len == 0) 
				continue;
			buffer[command_len] = '\0';
			--command_len;
			tab_option = 0;
			fputs("\b", stdout);
			fputs("\033[K", stdout);
		}

		/* Optional for UP or DOWN keys */
		else if (c == 27) {
			char *temp;
			c = getchar();
			c = getchar();
			tab_option = 0;
			switch (c) {
				/* Code for UP key */
				case 65:
					temp = get_prev_history_command();
					if (buffer != NULL) {
						seek_to_beginning(buffer);
					}
					free(buffer);
					buffer = strdup(temp);
					/* Remove the \n at the end of the command */
					buffer[strlen(buffer)-1] = '\0';
					command_len = strlen(buffer);
					printf("%s", buffer);
					break;
					
				/* Code for DOWN key */
				case 66:
					temp = get_next_history_command();
					if (buffer != NULL) {
						seek_to_beginning(buffer);
					}
					buffer = strdup(temp);
					/* Remove the \n at the end of the command */
					buffer[strlen(buffer)-1] = '\0';
					command_len = strlen(buffer);
					printf("%s", buffer);
					break;
				/* Code for RIGHT key */
				case 67:
					break;
				/* Code for LEFT key */
				case 68:
					break;
			}
		}

		/* Ignore other strange keys */
		else if (c >= 0 && c <= 31) {
			free(buffer);
			buffer = NULL;
			command_len = 0;
			actual_buffer_len = 0;
			tab_option = 0;
		}

		else {
			putchar(c);
			buffer[command_len] = c;
			++command_len;
			tab_option = 0;
		}		
	}

	/* This is for putting \n at the last char of the buffer for future use */
	len = strlen(buffer);
	final_command = (char *)malloc(len + 2);
	if (final_command == NULL) {
		perror("malloc: ");
		exit(EXIT_FAILURE);
	}
	strcpy(final_command, buffer);
	final_command[len] = '\n';
	final_command[len + 1] = '\0';

	free(buffer);
	buffer = NULL;

	return final_command;
}

void init_shell(){
	/* Check if we are running interactively */
	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty (shell_terminal); 
	if (shell_is_interactive) {
		/* Loop until we are in the foreground (while the real shell's pgrp is not our new pgrp) */
		while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
			kill (- shell_pgid, SIGTTIN); 
		/* Ignore interactive and job-control signals. */
		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		signal (SIGTTIN, SIG_IGN);
		signal (SIGTTOU, SIG_IGN);
		signal (SIGCHLD, SIG_IGN);  

		/* Put ourselves in our own process group */
		shell_pgid = getpid();
		if (setpgid (shell_pgid, shell_pgid) < 0)
		{
			perror ("Couldn't put the shell in its own process group");
			exit (1);
		} 
		/* Get control of the terminal */
		tcsetpgrp (shell_terminal, shell_pgid);
	}
}

void set_terminal_settings() {
	tcgetattr( STDIN_FILENO, &shell_tmodes_old);
	shell_tmodes_new = shell_tmodes_old;
	shell_tmodes_new.c_lflag &= ~(ICANON | ECHO | ISIG);
	tcsetattr( STDIN_FILENO, TCSANOW, &shell_tmodes_new);
}


/* Put job j in the foreground  
	 If cont is nonzero, restore the saved terminal modes and send the process group a
	 SIGCONT signal to wake it up before we block */
void put_job_in_foreground (job *j, int cont) {
	/* Put the job into the foreground */
	tcsetpgrp (shell_terminal, j->pgid);

	/* Send the job a continue signal, if necessary */
	if (cont)
	{
		printf("%s", j->command);
		tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
		if (kill (- j->pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");
	}

	/* Wait for it to report */
	wait_for_job (j);

	/* Put the shell back in the foreground */
	tcsetpgrp (shell_terminal, shell_pgid);

	/* Restore the shell’s terminal modes */
	tcgetattr (shell_terminal, &j->tmodes);
	tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes_old);
}


/* Put a job in the background  If the cont argument is true, send
	 the process group a SIGCONT signal to wake it up */
void put_job_in_background (job *j, int cont) {
	/* Send the job a continue signal, if necessary */
	if (cont) {
		if (kill (-j->pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");
	}
}

/* Get to the beggining of the last input and erase it */
void seek_to_beginning(char *buf) {
	for (uint i=0; i< strlen(buf);i++) {
		fputs("\b", stdout);
	}
	fputs("\033[K", stdout);
}

/* Print suggestions */
void print_all_suggestions(command *first_suggestion) {
	command *curr = first_suggestion;
	fputs("\n", stdout);
	while (curr) {
		printf("%s\t", curr->data);
		curr = curr->next;
	}
}

void free_all_suggestions(command *first_suggestion) {
	command *curr;
	while (first_suggestion) {
		curr = first_suggestion;
		first_suggestion = first_suggestion->next;
		free(curr);
	}
}