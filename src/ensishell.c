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
// #include <stdbool.h>
#include "jobNode.h"
void addNode(job *head, int pid, int flag, char *cmd)
{
    job *newNode = createNode(pid, flag, cmd);
    if (head->next == NULL)
    {
        head->next = newNode;
    }
    else
    {
        job *tempHead = head;
        while (tempHead->next != NULL)
        {
            tempHead = tempHead->next;
        }
        tempHead->next = newNode;
    }
}
job *createNode(int pid, int flag, char *cmd)
{
    job *newNode = NULL;
    newNode = (job *)malloc(sizeof(job));
    newNode->pid = pid;
    newNode->flag = flag;
    // if(strlen(cmd)>){

    // }
    strcpy(newNode->cmd, cmd);
    newNode->next = NULL;
    return newNode;
}
int size(job *head)
{
    int cont = 0;
    job *tempHead = head;
    while (tempHead != NULL)
    {
        cont++;
        tempHead = tempHead->next;
    }
    return cont;
}
void showList(job *head)
{
    job *tempHead = head->next;
    while (tempHead != NULL)
    {
        printf("%d\t%s\t%s\n", tempHead->pid, (waitpid(tempHead->flag,NULL,WNOHANG)==0)? "Running" : "Done", tempHead->cmd);
		// (waitpid(res,NULL,WNOHANG)==0)?0:1
        tempHead = tempHead->next;
    }
    printf("\n");
}

// typedef struct job{
// 	pid_t pid;
// 	bool flag; 
// 	char cmd[100];
// }job_t;
// #include <time.h>

// static job_t jobs[10] = {0};
// static int job_count = 0;

static job head = {.pid=-1,.next=NULL,.flag=-1,.cmd="\0"};
job* headPtr = &head;

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
		// job head = {.pid=-1,.next=NULL,.flag=-1,.cmd="\0"};
		// job* headPtr = &head;
		printf("%d\n",headPtr->pid);
		/* Display each command of the pipe */
		for (i = 0; l->seq[i] != 0; i++)
		{
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j = 0; cmd[j] != 0; j++)
			{
				printf("'%s' ", cmd[j]);
			}
			printf("\n");
			// printf("Main Proc: %d \n",getpid());
			// printf("%d\n", strcmp(cmd[i], "jobs"));
			if (strcmp(cmd[i], "jobs"))
			{
				pid_t res = fork();
				
				if (res > 0)
				{
					// printf("Main Proc: %d - Son: %d \n",getpid(), res);
					if(l->bg==0)
					{
						wait(NULL);
						// jobs[job_count++].flag = ;					
					}
					else
					{
						// jobs[job_count].pid = res;
						// strcpy(jobs[job_count].cmd, cmd[i]);
						// job_count++;
						addNode(headPtr,res,(waitpid(res,NULL,WNOHANG)==0)?0:1,cmd[i]);
						// addNode(headPtr,res,1,cmd[i]);
						
					}
				}
				else if (res == 0)
				{
					// printf(" - Son: %d - Main Proc: %d \n", getpid(), getppid());
					if (execvp(cmd[i], cmd) < 0) 
					{
						printf("\nCould not execute command\n");
					}
					// execvp(cmd[i], cmd);
					// printf("pid: %d\n", pid);
				}
				else
				{
					printf("error: %s", l->err);
				}
			}
			else
			{
				showList(headPtr);
				// for (int k = 0; k < size(head); k++)
				// {
				// 	// jobs[k].flag = !waitpid(jobs[k].pid, NULL, WNOHANG);
				// 	printf("%d\t%s\t%s\n", jobs[k].pid, jobs[k].flag ? "Running" : "Done", jobs[k].cmd);
				// }
			}		
		}
	}
}
