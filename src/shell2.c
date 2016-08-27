#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
        int q_size;
        int head;
        int tail;
        int commandSize;
        char **history;

    } historyQueue;


void  execute(char **argv);
void  parse(char *line, char **argv);

void init_queue(historyQueue *queue, int n);
void resize_queue(historyQueue *queue,int n);
void enqueue(historyQueue *queue, char *newCommand);
void dequeue(historyQueue *queue,char *buf);
void printQueue(historyQueue *queue);

//handle errors, segmentation faults etc.
int main(void){

    historyQueue q;
    init_queue(&q,4);
    // printf("%d  %p\n",q.tail,q.history);

    // printQueue(&q);
    // enqueue(&q,"1");
    // enqueue(&q,"2");
    // enqueue(&q,"3");
    // enqueue(&q,"4");
    // enqueue(&q,"5");














    //dynamically allocate memory instead
    // char line[1024];
    // int count=0, size, i;
    // //dynamically allocate, make configurable
    // char *hist[10];

    // //array of strings?
    // char  *argv[64];              
    
    // //initialize elsewhere
    // //introduce "environment"
    // //instead initialize to some x
    // for(i=0;i<10;i++)
    //     hist[i]=NULL;


    // while(1){
    //     //add user info here+pwd
    //     //READ
    //     printf(">>$:");

    //     //improve readline. first try using gnu c readline lib
    //     //then try implementing some of it.
    //     gets(line);

    //     //why?
    //     size=strlen(line);

    //     //make cleaner , what the hell is happening here?
    //     //why are we allocating exactly that much space? makes no sense!
    //     //overhead, completely change this part
    //     if(hist[count]!=NULL)
    //         hist[count]=(char*)realloc(hist[count], (size+1)*sizeof(char));
    //     else        
    //         hist[count]=(char *)malloc((size+1)*sizeof(char));
        
    //     //where is the queue implementation of history? 
    //     //how are we going to be able to rotate? checking for full buffer?
    //     {//ok, why the curly brackets?
    //         strcpy(hist[count], line);

    //         count++;}

        


    //     printf("\n");
    //     parse(line, argv); 
    //     //check if history? change this to check for builtin commands
    //     //!! and !n

    //     //implement cd , ls,bg,fg,kill,jobs,alias,wait,exit,clear,set,reset
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
    // }
    // return 0;
}



void  parse(char *line, char **argv)
{   
    
    //ok so basically removes the whitespace?
    //need a better parser!!
    // at the very lowest level , history? piping?
    //what does a parser even do?
    //tokenization?
    // while (*line != '\0') //end of line
    // {       
    //     while (*line == ' ' || *line == '\t' || *line == '\n')
    //         *line++ = '\0';     //why am i setting the next to \0?
        

    //     printf("got here\n");
    //     *argv++ = line;          
    //     // printf("ARG : %s\n",*argv);
    //     printf("didn't get here\n");
    //     while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') 
    //         line++;            
    //  }
    //  *argv = '\0';                 





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
    // queue->tail=3;
    queue->commandSize=100;
    queue->history=(char **) malloc(n*sizeof(char *));
    // printf("got here %d\n",n);
    for (i=0;i<n;i++){
        // printf("aaa\n");
        // printf("%p\n",*(queue->history+i));
        *((queue->history)+i)=(char *)malloc ((queue->commandSize)*sizeof(char));
        // printf("%p\n",*(queue->history+i));
        (*((queue->history)+i))[0]='\0';

        // (*((queue->history)+i))[0]='a';
        // (*((queue->history)+i))[1]='\0';
    }
    // printf("even here\n");
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
    printf("a\n");
    queue->tail = (queue->tail + 1)%(queue->q_size);
    printf("%d\n",queue->tail);
    //maxsize check here
    // char *dest= *(queue->history + queue->tail);
    char *dest=malloc(20*sizeof(char));
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
    if (queue->tail==-1){
        return ;
    }
    while( ((queue->head +i)%(queue->q_size))!= (queue->tail)){
            printf("%d . %s\n",i,*(queue->history+(queue->head +i)%(queue->q_size)));
            i++;
        }
}




