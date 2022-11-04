
#if !defined(__JOB_LIST__)
#define __JOB_LIST__

    typedef struct job{
        int pid;
	    int flag;
        char cmd[100]; 
	    struct job* next;
    }job;

    void addNode(job* head,int pid,int flag,char* cmd);
    job* createNode(int pid,int flag,char* cmd);
    int size(job* head);
    void showList(job* head);
#endif // __JOB_LIST__
