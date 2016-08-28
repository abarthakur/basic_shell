#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define ARGMAX 50


typedef struct {
        int q_size;
        int head;
        int tail;
        int commandSize;
        char **history;

    } historyQueue;


typedef void (*Command)(char **argv,historyQueue *q);

void history_cmd(char **argv,historyQueue *q);
int custom_execute(char **argv,historyQueue *q);
void  execute(char **argv);
void  parse(historyQueue *q, char *line);

void init_queue(historyQueue *queue, int n);
void resize_queue(historyQueue *queue,int n);
void enqueue(historyQueue *queue, char *newCommand);
void dequeue(historyQueue *queue,char *buf);
void printQueue(historyQueue *queue);

//handle errors, segmentation faults etc.
int main(void){

    historyQueue q;
    init_queue(&q,4);
    //dynamically allocate memory instead
    char *line= (char *) malloc(1024 * sizeof(char));
    //array of strings?
    // char  *argv[64];              
    
    //introduce "environment"

    while(1){
        //add user info here+pwd
        //READ
        printf("\n>>$:");
        //improve readline. first try using gnu c readline lib
        //then try implementing some of it.
        gets(line);

        //add to history
        enqueue(&q, line);
        
        parse(&q,line); 
        //check if history? change this to check for builtin commands
        //!! and !n

        //implement cd , ls,bg,fg,kill,jobs,alias,wait,exit,clear,set,reset
    //     if(strcmp(argv[0], "history")==0)
    //     {
    //         for(i=0;i<count;i++)
    //         {   
    //             if(hist[i]!=NULL)           
    //                 printf("%s\n",hist[i]);
    //         }
    //         printf("\n");
    //         continue;
    //     }

    //         if (strcmp(argv[0], "exit") == 0)  
    //             break; //return from shell to main          
    //         execute(argv);          
    }
    return 0;
}



void  parse(historyQueue *q, char *line)
{   
    
    //ok so basically removes the whitespace?
    //need a better parser!!
    // at the very lowest level , history? piping?
    //what does a parser even do?
    //tokenization?
    int i,count=0;
    char *mybuf = (char *)malloc(1024*sizeof(char));
    strcpy(mybuf,line);
    char **argv=malloc(ARGMAX*sizeof(char *));
    for (i=0;i<ARGMAX;i++){
        argv[i]=NULL;
    }
    int start=0;
    while (*(mybuf) != '\0') //end of mybuf
    {       
        if (*(mybuf) == ' ' || *(mybuf) == '\t' || *(mybuf) == '\n'){
            // printf("1\n");
            start=0;
            *(mybuf) = '\0';     
        }
        else if (*(mybuf) == '>' || *(mybuf)== '|' ){
            // printf("2\n");
            start==0;
        }
        else{
            // printf("3\n");
            if (start ==0){
                // printf("4\n");
                start =1;
                argv[count]=mybuf;
                count++;
            }
        }        
        mybuf++;
    }
    i=0;
    while(argv[i]!=NULL){
        // printf("ARG %d. %s\n ",i+1,argv[i]);
        i++;
    }

    char *repeat=(char *)malloc((q->commandSize)*sizeof(char));
    char *src;
    int x;

    if(argv[0][0]=='!'){
        if ( strcmp(argv[0],"!!")==0){
            // printf("here\n");
            q->tail= (q->tail -1)%(q->q_size);
            strcpy(repeat,*(q->history+q->tail));        
            enqueue(q,repeat);
            return parse(q,repeat);
        }
        else {//assume that it will come correctly
            x=atoi(argv[0]+1);
            if(x!=0){
                src= *(q->history + (q->head + (x-1))%(q->q_size));
                strcpy(repeat,src);
                q->tail= (q->tail -1)%(q->q_size);
                enqueue(q,repeat);
                return parse(q,repeat);       
            }
        }
        return;
    }

    if (custom_execute(argv,q)==1){
        execute(argv);
    }
}

int custom_execute(char **argv,historyQueue *q){
    // char **custom_commands={"pwd","cd","jobs","bg","history"};
    const char *custom_commands[]={"history"};
    // Command *custom = {NULL. NULL, NULL , NULL, NULL, NULL};
    Command custom[]= {history_cmd};
    int cust_no=1;
    int j=0;
    // return 1;//comment out later
    for (j=0;j<cust_no;j++){
        if (strcmp(argv[0],custom_commands[j])==0){
            custom[j](argv,q);       
            return 0;
        }
    }
    

}


void history_cmd(char **argv,historyQueue *q){
    printQueue(q);
}


void  execute(char **argv)
{
    pid_t  pid;
    int    status;
    //give later, store pids.
    // while(*(argv+i)!=NULL){
    //     printf("ARG :%s\n",*(argv+i));
    // }
    pid = fork();
    if (pid < 0) 
    {     
        printf("ERROR:-2\n");
        return;
    }
    else if (pid == 0)
    {          
        if (execvp(argv[0], argv) < 0)//execvp?
        {     
            printf("ERROR: -1\n");
            return;
        }
    }
    else
    {                                 
        while (wait(&status) != pid);
    }
}          




































void init_queue(historyQueue *queue, int n){
    int i=0;
    
    queue->q_size=n;
    queue->head=0;
    queue->tail=-1;
    queue->commandSize=100;
    queue->history=(char **) malloc(n*sizeof(char *));
    for (i=0;i<n;i++){
        *((queue->history)+i)=(char *)malloc ((queue->commandSize)*sizeof(char));
        (*((queue->history)+i))[0]='\0';

    }
}

void resize_queue(historyQueue *queue,int n){
    char **newHistory=(char **) malloc(n*sizeof(char *)); 
    int i;

    if (n > queue->q_size){//upsize
        for (i=0; i <queue->q_size; i++){
            *(newHistory+i)=*(queue->history+(queue->head +i)%(queue->q_size));
        }
        for (i=queue->q_size;i<n;i++){
            *(newHistory+i)=malloc ((queue->commandSize)*sizeof(char *));
        }
        free(queue->history);
        queue->history=newHistory;
        if (queue->tail < queue->head){//can never be equal. if less, less only by 1, since dequeue is called only from enqueue . necessity of dequeue? if greater, head=0
            queue->tail = queue->q_size -1 - queue->tail;
        }
        queue->q_size = n;
        queue->head=0;
    }

    else if (n < queue->q_size){
        for (i=0; i <n;i++){
           *(newHistory+i)=*(queue->history+(queue->head +i));
        }
        for (i=n;i<queue->q_size;i++){
            free(*(queue->history+(queue->head +i)%(queue->q_size)));    
        }
        free(queue->history);
        queue->history=newHistory;
        if (queue->tail < queue->head || queue->tail >= n ){//can never be equal. if less, less only by 1, since dequeue is called only from enqueue . necessity of dequeue?
            queue->tail = queue->q_size - 1;
        }
        queue->q_size = n;
        queue->head=0;
    }
}

void enqueue(historyQueue *queue, char *newCommand){
    // int head =queue->head;
    
    // int tail = queue->tail;
    if((queue->tail +1)%(queue->q_size)==queue->head && queue->tail!=-1){ //boundary condition
        dequeue(queue,NULL);
    }
    // printf("a\n");
    queue->tail = (queue->tail + 1)%(queue->q_size);
    // printf("%d\n",queue->tail);
    //maxsize check here
    char *dest= *(queue->history + queue->tail);
    // char *dest=malloc(20*sizeof(char));
    strcpy(dest,newCommand);
}

void dequeue(historyQueue *queue,char *buf){
    if (buf!=NULL){
        strcpy(buf,*(queue->history + queue->head));
    }
    queue->head=(queue->head+1)%(queue->q_size);
}


void printQueue(historyQueue *queue){
    int i;
    printf("History :\n");
    if (queue->tail==-1){
        return ;
    }

    int start=1;
    while( start==1 ||(((queue->head +i)%(queue->q_size))!= (queue->tail +1)%(queue->q_size))){
            start=0;
            printf("%d . %s\n",i,*(queue->history+(queue->head +i)%(queue->q_size)));
            i++;
        }
}




