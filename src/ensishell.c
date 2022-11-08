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
#include <errno.h>

job head = {.pid = -1, .next = NULL, .flag = -1, .cmd = "\0"};
job *headPtr = &head;
// job* headPtr = NULL;

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

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
	printf("Not implemented yet: can not execute %s\n", line);

	printf("Here ...");

	/* Remove this line when using parsecmd as it will free it */
	free(line);

	return 0;
}

SCM executer_wrapper(SCM x)
{
	return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}
#endif

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
		int i, j;
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

		if (l->in)
			printf("in: %s\n", l->in);
		if (l->out)
			printf("out: %s\n", l->out);
		if (l->bg)
			printf("background (&)\n");

		// int contComandos = 0;
		// int contLetraComando = 0;
		// int flag;
		/* Display each command of the pipe */
		// printf("%c",*(l->seq[1]));
		for (i = 0; l->seq[i] != 0; i++)
		{
			// printf("%s",l->seq[1]);
			char **cmd = l->seq[i];
			// contComandos=0;
			// printf("size of: %ld\n",sizeof(cmd));
			// printf("in: %s\n", l->in);
			// printf("out: %s\n", l->out);
			// printf("bg: %d\n", l->bg);
			// for (j = 0; cmd[j] != 0; j++)
			// {
			// 	printf("seq[%d]: ", i);
			// 	printf("'%s' ", cmd[j]);

			// }

			for (j = 0; cmd[j] != 0; j++)
			{
				// contComandos++;
				// contLetraComando =0 ;
				printf("seq[%d]: ", i);
				printf("'%s' ", cmd[j]);
				// char* cmdTemp = cmd[j];
				// while(*cmdTemp!='\0'){
				// 	contLetraComando++;
				// 	printf("\nAaqui testing: %c\n",*cmdTemp);
				// 	cmdTemp++;
				// }
				// printf("despues: %c\n",*cmdTemp);
				// printf("\ncontador letra comandos: %d",contLetraComando);
			}
			// printf("\ncontador comandos: %d",contComandos);
			printf("\n");

			// if (i == 1)
			// {
			// 	char **cmd_1 = l->seq[0];
			// 	char **cmd_2 = l->seq[1];
			// 	int fdp[2];
			// 	pipe(fdp);
			// 	int res = fork();
			// 	if (res == 0)
			// 	{
			// 		dup2(fdp[0], 0);
			// 		close(fdp[1]);
			// 		close(fdp[0]);
			// 		execvp(cmd_2[0], cmd_2);
			// 	}
			// 	else
			// 	{
			// 		dup2(fdp[1], 1);
			// 		close(fdp[0]);
			// 		close(fdp[1]);
			// 		execvp(cmd_1[0], cmd_1);
			// 	}
			// }

			// if (strcmp(cmd[i], "jobs"))
			// {
			// 	// int pipefds[2];
			// 	// if (pipe(pipefds) == -1)
			// 	// {
			// 	// 	printf("error");
			// 	// }
			// 	// if(cmd[i+1]!=NULL){
			// 	// 	int pipefds[2];
			// 	// 	if (pipe(pipefds)==-1)
			// 	// 	{
			// 	// 		printf("error");
			// 	// 		return 1;
			// 	// 	}
			// 	// 	int subProcess = fork();
			// 	// 	if(subProcess==0){
			// 	// 		dup2(pipefds[0],STDIN_FILENO);
			// 	// 		close(pipefds[0]);
			// 	// 		close(pipefds[1]);
			// 	// 		execvp(cmd[i+1],cmd[]);
			// 	// 	}else{
			// 	// 		close();
			// 	// 		dup2(pipefds[0],STDIN_FILENO);
			// 	// 		execvp()
			// 	// 	}

			// 	// }
			// 	pid_t res = fork();
			// 	// padre process
			// 	if (res > 0)
			// 	{
			// 		if (l->bg == 0)
			// 		{
			// 			wait(NULL);
			// 			// while(wait(NULL)!=-1 ||errno !=ECHILD){
			// 			// 	printf("waiting");
			// 			// }
			// 		}
			// 		else
			// 		{
			// 			addNode(headPtr, res, (waitpid(res, NULL, WNOHANG) == 0) ? 0 : 1, cmd[i]);
			// 		}
			// 	}
			// 	else if (res == 0)
			// 	{

			// 		// Que haga esto en caso de q se necesite
			// 		// conectar l info con un separador como
			// 		// if(strchr(cmd[i],'|')||strchr(cmd[i],'>')||strchr(cmd[i],'<')){
			// 		// 	int res2 = fork();
			// 		// 	if(res2==0)
			// 		// 	{
			// 		// 		printf("jajajajjaaj");
			// 		// 	}else
			// 		// 	{
			// 		// 		printf("vgggggg");
			// 		// 	}
			// 		// }
			// 		// dup2(pipefds[])
			// 			if (execvp(cmd[i], cmd) < 0)
			// 			{
			// 			printf("\nCould not execute command\n");
			// 			}

			// 	}
			// 	else
			// 	{
			// 		printf("error: %s", l->err);
			// 	}
			// }
			// else
			// {
			// 	showList(headPtr);
			// }
		}
		if (l->seq[1]!=NULL)
		{
			// int parentId = getpid();
			char **cmd_1 = l->seq[0];
			char **cmd_2 = l->seq[1];
			// int old_stdin = dup(STDIN_FILENO);
			// int old_stdout = dup(STDOUT_FILENO);
			int fdp[2];
			pipe(fdp);
			int res = fork();
			if (res == 0)
			{
				dup2(fdp[0], 0);
				close(fdp[1]);
				close(fdp[0]);
				execvp(cmd_2[0], cmd_2);
				// dup2(old_stdout, STDOUT_FILENO);
			}
			else
			{
				dup2(fdp[1], 1);
				close(fdp[0]);
				close(fdp[1]);
				execvp(cmd_1[0], cmd_1);
				// dup2(old_stdin, STDIN_FILENO);
			}
			// wait(NULL);
			//waitpid(parentId,);
		}else{
			char **cmd = l->seq[0];
			if (strcmp(cmd[0], "jobs"))
			{
				pid_t res = fork();
				// padre process
				if (res > 0)
				{
					if (l->bg == 0)
					{
						wait(NULL);
						// while(wait(NULL)!=-1 ||errno !=ECHILD){
						// 	printf("waiting");
						// }
					}
					else
					{
						addNode(headPtr, res, (waitpid(res, NULL, WNOHANG) == 0) ? 0 : 1, cmd[0]);
					}
				}
				else if (res == 0)
				{

					// Que haga esto en caso de q se necesite
					// conectar l info con un separador como
					// if(strchr(cmd[i],'|')||strchr(cmd[i],'>')||strchr(cmd[i],'<')){
					// 	int res2 = fork();
					// 	if(res2==0)
					// 	{
					// 		printf("jajajajjaaj");
					// 	}else
					// 	{
					// 		printf("vgggggg");
					// 	}
					// }
					// dup2(pipefds[])
						if (execvp(cmd[0], cmd) < 0)
						{
						printf("\nCould not execute command\n");
						}

				}
				else
				{
					printf("error: %s", l->err);
				}
			}
			else
			{
				showList(headPtr);
			}
		}
	}
}
