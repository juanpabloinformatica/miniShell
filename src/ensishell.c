/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variante.h"
#include "readcmd.h"
#include <unistd.h>
#include <sys/wait.h>
#include "jobNode.h"
#include "joker_glob.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>

job head = {.pid = -1, .next = NULL, .flag = -1, .cmd = "\0"};
job *headPtr = &head;
int limit;
// job* headPtr = NULL;

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

void execute_command(char **cmd, int bg, char *err) {
	if (strcmp(cmd[0], "jobs"))
	{
		pid_t res = fork();
		if (res > 0)
		{
			if (bg == 0)
			{
				wait(NULL);
			}
			else
			{
				addNode(headPtr, res, (waitpid(res, NULL, WNOHANG) == 0) ? 0 : 1, cmd[0]);
			}
		}
		else if (res == 0)
		{
				if (execvp(cmd[0], cmd) < 0)
				{
				printf("\nCould not execute command\n");
				}
				perror("execvp");
				_exit(1);
		}
		else
		{
			printf("error: %s", err);
		}
	}
	else
	{
		showList(headPtr);
	}
}

void terminate(char *line)
{
#if USE_GNU_READLINE == 1
	/* rl_clear_history() does not exist yet in centOS 6 */
	clear_history();
#endif
	if (line)
		free(line);
	printf("exit\n");
	exit(0);
}

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */

#if USE_GUILE == 1
#include <libguile.h>

int question6_executer(char *line)
{
	/* Question 6: Insert your code to execute the command line
	 * identically to the standard execution scheme:
	 * parsecmd, then fork+execvp, for a single command.
	 * pipe and i/o redirection are not required.
	 */
	struct cmdline *l = parsecmd(&line);
	/* If input stream closed, normal termination */
	if (!l)
	{
		terminate(0);
	}
	if (l->err)
	{
		/* Syntax error, read another command */
		printf("error: %s\n", l->err);
		return EXIT_FAILURE;
	}
	execute_command(l->seq[0], l->bg, l->err);
	free(line);

	return 0;
}

SCM executer_wrapper(SCM x)
{
	return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}
#endif

int main()
{
	printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
	scm_init_guile();
	/* register "executer" function in scheme */
	scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

	while (1)
	{
		// job* headPtr = NULL;
		// headPtr = (job*)malloc(sizeof(job));
		// printf("%d:\t%p:\t",headPtr->pid,headPtr->next);
		// printf("holaaaa");
		struct cmdline *l;
		char *line = 0;
		// int i, j;
		char *prompt = "ensishell>";
		/* Readline use some internal memory structure that
		   can not be cleaned at the end of the program. Thus
		   one memory leak per command seems unavoidable yet */
		line = readline(prompt);
		if (line == 0 || !strncmp(line, "exit", 4))
		{
			terminate(line);
		}

#if USE_GNU_READLINE == 1
		add_history(line);
#endif

#if USE_GUILE == 1
		/* The line is a scheme command */
		if (line[0] == '(')
		{
			char catchligne[strlen(line) + 256];
			sprintf(catchligne, "(catch #t (lambda () %s) (lambda (key . parameters) (display \"mauvaise expression/bug en scheme\n\")))", line);
			scm_eval_string(scm_from_locale_string(catchligne));
			free(line);
			continue;
		}
#endif

		/* parsecmd free line and set it up to 0 */
		l = parsecmd(&line);

		/* If input stream closed, normal termination */
		if (!l)
		{
			terminate(0);
		}
		if (l->err)
		{
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		// if (l->in)
		// 	printf("in: %s\n", l->in);
		// if (l->out)
		// 	printf("out: %s\n", l->out);
		// if (l->bg)
		// 	printf("background (&)\n");

		/* Display each command of the pipe */
		// for (int i=0; l->seq[i]!=0; i++) {
		// 	char **cmd = l->seq[i];
		// 	printf("seq[%d]: ", i);
        //                 for (int j=0; cmd[j]!=0; j++) {
        //                         printf("'%s' ", cmd[j]);
        //                 }
		// 	printf("\n");
		// 	// checkJoker(l->seq[i]);
		// }

		// checkJoker(l->seq[0]);
		checkJoker(l->seq);
		char **cmd = l->seq[0];
		int old_stdin = dup(STDIN_FILENO);
		int old_stdout = dup(STDOUT_FILENO);
		int input;
		int output;
		// if(cmd)
		// char unlimitS[100] = "";
		// int X;
		if(strcmp(cmd[0],"unlimit\0")==0){
			if(cmd[1]==NULL){
				printf("insertez le limit X");
			}else{
				printf("%d",atoi(cmd[1]));
				limit = atoi(cmd[1]);
				continue;
			}
		}
		if (strcmp(cmd[0], "jobs")) {
			int res = fork();
			if (res == 0) {
				struct rlimit rLimit;
				getrlimit(RLIMIT_CPU,&rLimit);
				rLimit.rlim_cur = limit+5;
				setrlimit(RLIMIT_CPU,&rLimit);
				// setrlimit(CPU);
				if (l->in) {
					input = open(l->in, O_RDONLY);
					dup2(input, 0);
					close(input);
				}
				if (l->out) {
					output = open(l->out, O_RDWR | O_CREAT, 0644);
					dup2(output, 1);
					close(output);
					//ftruncate(output, 0);
				}
				if (l->seq[1] != NULL) {
					char **cmd_2 = l->seq[1];
					int fdp[2];
					pipe(fdp);
					int res2 = fork();
					if (res2 == 0) {
						dup2(fdp[0], 0);
						close(fdp[1]);
						close(fdp[0]);
						execvp(cmd_2[0], cmd_2);
						perror("execvp");
						_exit(1);
					}
					dup2(fdp[1], 1);
					close(fdp[0]);
					close(fdp[1]);
				}
				execvp(cmd[0], cmd);
				perror("execvp");
				_exit(1);
			} else if (res > 0) {
				if (l->bg == 0)
				{
					wait(NULL);
				}
				else
				{
					char cmdComplete[999]="";
					for(int i = 0; cmd[i]!=NULL;i++){
						strcat(cmdComplete," ");
						strcat(cmdComplete,cmd[i]);
					}
					strcat(cmdComplete," &");
					addNode(headPtr, res, (waitpid(res, NULL, WNOHANG) == 0) ? 0 : 1, cmdComplete);
				}
			} else {
				printf("error: %s", l->err);
			}
			dup2(old_stdin, 0);
			dup2(old_stdout, 1);
			close(old_stdin);
			close(old_stdout);
		} else {
			showList(headPtr);
		}
	}
}
