#include "jobNode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

// job head = {.pid=-1,.next=NULL,.flag=-1,.cmd="\0"};
// extern job* headPtr = &head;
// head
int count = 0;
char command[100]; 
int pidCommand;

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
        // newNode->next = head->next;
        // head->next = newNode->next
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
    // int i = 0;
    while (tempHead != NULL)
    {
        tempHead->flag = waitpid(tempHead->flag, NULL, WNOHANG);
        // printf("why: %d\n", tempHead->flag);
        if (tempHead->flag != 0)
        {
            deleteNode(head,tempHead->pid);
            // printf("%d: %d\n", (i + 1), tempHead->flag);
            // printf("%d",tempHead->flag);
            printf("%d\t%s\t%s\n", pidCommand, "Done", command);
            strcpy(command,"");
        }
        else
        {
            // printf("%d: %d\n", (i + 1), tempHead->flag);

            printf("%d\t%s\t%s\n", tempHead->pid, "Running", tempHead->cmd);
        }
        tempHead = tempHead->next;
    }
    // printf("\n");
}
void deleteNode(job *head, int pid)
{
    if (count!=1)
    {
        job *tempHead = head;
        job *antTempHead = createNode(-2, -1, "\0");
        antTempHead->next = tempHead;
        // job head = {.pid = -1, .next = NULL, .flag = -1, .cmd = "\0"};
        while (tempHead->pid != pid)
        {
            antTempHead = antTempHead->next;
            tempHead = tempHead->next;
        }
        
        if (tempHead != NULL)ead->cmd);
        {
            // printf("cmd : %s",tempHead->cmd);
            strcpy(command,tempHead->cmd);
            pidCommand = tempHead->pid;
            antTempHead->next = tempHead->next;
            free(tempHead);
        }
        count=0;
    }else{
        count++;
    }
}