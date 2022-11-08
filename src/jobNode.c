#include "jobNode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

// job head = {.pid=-1,.next=NULL,.flag=-1,.cmd="\0"};
// extern job* headPtr = &head;
// head
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
void showList(job* head)
{
    job *tempHead = head->next;
    while (tempHead != NULL)
    {
        printf("%d\t%s\t%s\n", tempHead->pid, (waitpid(tempHead->flag,NULL,WNOHANG)==0)? "Running" : "Done", tempHead->cmd);
        tempHead = tempHead->next;
    }
    // printf("\n");
}