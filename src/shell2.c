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


typedef void (*Command)(char **argv,historyQueue *q,FILE *from,FILE *to);

void history_cmd(char **argv,historyQueue *q,FILE *from, FILE *to);
void present_wd(char **argv,historyQueue *q,FILE *from, FILE *to);
void change_dir(char **argv,historyQueue *q,FILE *from, FILE *to);


int custom_execute(char **argv,historyQueue *q,FILE *from, FILE *to);
void  execute(char **argv,FILE *from, FILE *to);
void  parse(historyQueue *q, char *line);

void init_queue(historyQueue *queue, int n);
historyQueue *resize_queue(historyQueue *queue,int n);
void enqueue(historyQueue *queue, char *newCommand);
void dequeue(historyQueue *queue,char *buf);
void printQueue(historyQueue *queue,FILE *to);

int main(void){

    historyQueue q;
    init_queue(&q,10);
    char *line= (char *) malloc(1024 * sizeof(char));
    //introduce "environment"

    while(1){
        printf("\n>>$:");
        gets(line);

        parse(&q,line); 
    }
    return 0;
}



void  parse(historyQueue *q, char *line)
{   
    
    int redirect=0, i,count=0;
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
            start=0;
            *(mybuf) = '\0';     
        }
        else if (*(mybuf) == '>' || *(mybuf) == '<' ||*(mybuf) == '|' ){
            redirect =count;
            argv[count]=(char *)malloc(2*sizeof(char));
            argv[count][0]=*mybuf;
            argv[count][1]='\0';
            count++;
            start==0;
        }
        
        else{
            if (start ==0){
                start =1;
                argv[count]=mybuf;
                count++;
            }
        }        
        mybuf++;
    }
    i=0;
    //list of args terminated by null pointer not \0
    argv[count]=NULL;

    if (argv[0]==NULL){
        return;
    }

    char *repeat=(char *)malloc((q->commandSize)*sizeof(char));
    char *src;
    int x;


    if(argv[0][0]=='!'){
        if ( strcmp(argv[0],"!!")==0){
            strcpy(repeat,*(q->history+q->tail));        
            enqueue(q,repeat);
            return parse(q,repeat);
        }
        else {
            x=atoi(argv[0]+1);
            if(x!=0){
                src= *(q->history + (q->head + (x-1))%(q->q_size));
                strcpy(repeat,src);
                q->tail= (q->tail -1)%(q->q_size);
                enqueue(q,repeat);
                return parse(q,repeat);       
            }
        }
        enqueue(q,line);
        return;
    }

    char *redirection;
    enqueue(q,line);
    FILE *to;
    FILE *from;
    if (redirect!=0){
        redirection=argv[redirect];
        if (strcmp(redirection,"<")==0){
            argv[redirect]=NULL;
            from=fopen(argv[redirect+1],"r");
            to=NULL;
        }

        else if (strcmp(redirection,">")==0){
            argv[redirect]=NULL;
            to=fopen(argv[redirect+1],"w");
            from=NULL;
        }

        else if (strcmp(redirection,"|")==0){
            argv[redirect]=NULL;
            from=NULL;
            to=fopen("pipeout","w");
            if (custom_execute(argv,q,from,to)==1){
                execute(argv,from,to);
            }
            to=NULL;
            from=fopen("pipeout","r");
            if (custom_execute(argv+redirect+1,q,from,to)==1){
                execute(argv+redirect+1,from,to);
            }
            return;
        }
    }    
    else {
        from=NULL;
        to=NULL;
    }
    
    if (custom_execute(argv,q,from,to)==1){
        execute(argv,from,to);
    }
}

void  execute(char **argv,FILE *from, FILE *to)
{
    pid_t  pid;
    int    status;
    pid = fork();
    if (pid < 0) {     
        printf("ERROR:-2\n");
        return;
    }
    else if (pid==0){
        if (from !=NULL){
            dup2(fileno(from), fileno(stdin));    
            close(fileno(from));
        }
        if (to !=NULL){
            dup2(fileno(to), fileno(stdout));    
            close(fileno(to));
        }
        if (execvp(argv[0], argv) < 0){     
            printf("ERROR: -1\n");
            return;
        }
    }
    else{
        while (wait(&status) != pid);
    }
}          




int custom_execute(char **argv,historyQueue *q,FILE *from,FILE *to){
    if (from==NULL){
        from =stdin;
    }
    if (to==NULL){
        to =stdout;
    }
    const char *custom_commands[]={"history","pwd","cd"};
    Command custom[]= {history_cmd, present_wd,change_dir};
    int cust_no=3;
    int j=0;
    for (j=0;j<cust_no;j++){
        if (strcmp(argv[0],custom_commands[j])==0){
            custom[j](argv,q,from,to);       
            return 0;
        }
    }
    return 1;
    

}


void history_cmd(char **argv,historyQueue *q,FILE *from, FILE *to){
    printQueue(q,to);
}

void present_wd(char **argv,historyQueue *q,FILE *from, FILE *to){
    char *pwd=(char *)get_current_dir_name();
    fprintf(to, "%s\n",pwd );
    fflush(to);
}

void change_dir(char **argv,historyQueue *q,FILE *from, FILE *to){
    if (argv[1]==NULL){
        argv[1]="/home/krysis/";
    }
    chdir(argv[1]);
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

historyQueue *resize_queue(historyQueue *queue,int n){
    historyQueue *newQueue=(historyQueue *)malloc(sizeof(historyQueue));
    init_queue(newQueue,n);
    if (queue->tail==-1){
        return newQueue;
    }
    int start=1,i=0;
    while( start==1 ||(((queue->head +i)%(queue->q_size))!= (queue->tail +1)%(queue->q_size))){
            start=0;
            enqueue(newQueue,*(queue->history+(queue->head +i)%(queue->q_size)));
            i++;
        } 

    return newQueue;
}

void enqueue(historyQueue *queue, char *newCommand){
    if((queue->tail +1)%(queue->q_size)==queue->head && queue->tail!=-1){ //boundary condition
        dequeue(queue,NULL);
    }
    queue->tail = (queue->tail + 1)%(queue->q_size);
    //maxsize check here
    char *dest= *(queue->history + queue->tail);
    strcpy(dest,newCommand);
}

void dequeue(historyQueue *queue,char *buf){
    if (buf!=NULL){
        strcpy(buf,*(queue->history + queue->head));
    }
    queue->head=(queue->head+1)%(queue->q_size);
}


void printQueue(historyQueue *queue,FILE *to){
    int i=0;
    fprintf(to,"History :\n");
    if (queue->tail==-1){
        return ;
    }

    int start=1;
    while( start==1 ||(((queue->head +i)%(queue->q_size))!= (queue->tail +1)%(queue->q_size))){
            start=0;
            fprintf(to,"%d . %s\n",i+1,*(queue->history+(queue->head +i)%(queue->q_size)));
            i++;
        }
    fflush(to);
}
