#include "funcs.h"

extern pid_t shell_pgid;
extern int shell_terminal;
extern int shell_is_interactive;
extern struct termios shell_tmodes_old;
extern struct termios shell_tmodes_new;

/* Handles the input loop */
char *handle_input() {
	char c;
	int len;
	int command_len, actual_buffer_len;
	char *buffer = NULL, *final_command = NULL;


	while ((c = getchar())) {
		
		if (buffer == NULL) {
			// printf("the buffer is NULL\n");
			buffer = (char *)malloc(sizeof(char *)*2);
			if (buffer == NULL) {
				perror("malloc: ");
				exit(EXIT_FAILURE);
			}
			command_len = 0;
			actual_buffer_len = 1;
		}

		else if (command_len + 1 >= actual_buffer_len) {
			// printf("reallocating buffer to size %d\n", actual_buffer_len*2 );
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
			break;
		}
		/* End of input */
		else if (c == 4) {
			buffer = strdup("exit");
			command_len = strlen(buffer);
			break;
		}
		/* Backslash */
		else if (c == 127) {
			if (command_len == 0) 
				continue;
			buffer[command_len] = '\0';
			--command_len;
			printf("\b");
			printf("\033[K");
		}

		/* Optional for UP or DOWN keys */
		else if (c == 27) {
			char *temp;
			c = getchar();
			c = getchar();
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

		else {
			putchar(c);
			buffer[command_len] = c;
			++command_len;
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
	shell_tmodes_new.c_lflag &= ~(ICANON | ECHO); 
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
		printf("\b");
	}
	printf("\033[K");
}