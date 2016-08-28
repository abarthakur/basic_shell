#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <ctype.h>

#define ARGMAX 50

typedef struct {
        int q_size;
        int head;
        int tail;
        int commandSize;
        int no_elem;
        char **history;

    } historyQueue;


typedef void (*Command)(char **argv,FILE *from,FILE *to);

void history_cmd(char **argv,FILE *from, FILE *to);
void present_wd(char **argv,FILE *from, FILE *to);
void change_dir(char **argv,FILE *from, FILE *to);


int custom_execute(char **argv,FILE *from, FILE *to);
void  execute(char **argv,FILE *from, FILE *to);
int  parse( char *line);

historyQueue *init_queue(int n);
historyQueue *resize_queue(historyQueue *queue,int n);
void enqueue(historyQueue *queue, char *newCommand);
void dequeue(historyQueue *queue,char *buf);
void printQueue(historyQueue *queue,FILE *to);


void shortPWD(void);

historyQueue *hist_q;
const char *PIPEOUT;
const char *HOME;
const char *USER;
const char *PWD;
char *shorterPWD;
int dirChange;

int main(void){

    shorterPWD=NULL;
    if ((HOME = getenv("HOME")) == NULL) {
       HOME = getpwuid(getuid())->pw_dir;
    }
    USER=getenv("USER");
    PIPEOUT=(char *)malloc(200);
    strcpy(PIPEOUT,HOME);
    strcat(PIPEOUT,"/pipeout");
    // printf("%s\n",PIPEOUT);
    dirChange=1;
    hist_q=init_queue(10);
    char *line= (char *) malloc(1024 * sizeof(char));
    //introduce "environment"

    while(1){
        if(dirChange==1){
            dirChange=0;
            PWD = (const char *)get_current_dir_name();
            shortPWD();
        }
        printf("\n%s:%s$:",USER,shorterPWD);
        gets(line);

        if (parse(line)==-1){
            //cleanup
            break;
        }
    }
    return 0;
}


void shortPWD(){
    free(shorterPWD);
    shorterPWD= (char *)malloc(strlen(PWD)*sizeof(char));
    // printf("here\n");
    int i =0;
    while (*(HOME+i)==*(PWD+i) && i <strlen(HOME)){
        i++;
    }
    if (i==strlen(HOME)){
        shorterPWD[0]='~';
        shorterPWD[1]='\0';
        strcat(shorterPWD,PWD+i);
    }
    else{
        strcpy(shorterPWD,PWD);
    }

}   

int  parse(char *line)
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
        return 1;
    }

    if (strcmp(argv[0],"exit")==0){
        printf("Shell will now exit\n");
        return -1;
    }

    char *repeat=(char *)malloc((hist_q->commandSize)*sizeof(char));
    char *src;
    int x;


    if(argv[0][0]=='!'){
        if ( strcmp(argv[0],"!!")==0){
            strcpy(repeat,*(hist_q->history+hist_q->tail));        
            enqueue(hist_q,repeat);
            return parse(repeat);
        }
        else {
            x=atoi(argv[0]+1);
            if(x<=hist_q->no_elem && x>0){
                src= *(hist_q->history + (hist_q->head + (x-1))%(hist_q->q_size));
                strcpy(repeat,src);
                enqueue(hist_q,repeat);
                return parse(repeat);       
            }
            else{
                printf("Invalid value of n. Use command !n to execute nth instruction in history.\n");
                return 1;
            }

        }
        enqueue(hist_q,line);
        return 1;
    }

    char *redirection;
    enqueue(hist_q,line);
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
            to=fopen(PIPEOUT,"w");
            if (custom_execute(argv,from,to)==1){
                execute(argv,from,to);
            }
            to=NULL;
            from=fopen(PIPEOUT,"r");
            if (custom_execute(argv+redirect+1,from,to)==1){
                execute(argv+redirect+1,from,to);
            }
            return 1;
        }
    }    
    else {
        from=NULL;
        to=NULL;
    }
    
    if (custom_execute(argv,from,to)==1){
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




int custom_execute(char **argv,FILE *from,FILE *to){
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
            custom[j](argv,from,to);       
            return 0;
        }
    }
    return 1;
    

}


void history_cmd(char **argv,FILE *from, FILE *to){
    if (argv[1]==NULL){
        printQueue(hist_q,to);
        return;
    }

    if (strcmp(argv[1],"--history-size")==0){
        int x=atoi(argv[2]);
        if (x != 0){
            hist_q = resize_queue(hist_q,x);
        }
    }

}

void present_wd(char **argv,FILE *from, FILE *to){
    char *pwd=(char *)get_current_dir_name();
    fprintf(to, "%s\n",pwd );
    fflush(to);
}

void change_dir(char **argv,FILE *from, FILE *to){
    dirChange=1;
    if (argv[1]==NULL){
        argv[1]=HOME;
    }
    chdir(argv[1]);
}



historyQueue *init_queue(int n){
    int i=0;
    historyQueue *queue= (historyQueue *)malloc(sizeof(historyQueue));
    queue->q_size=n;
    queue->head=0;
    queue->tail=-1;
    queue->commandSize=100;
    queue->no_elem=0;
    queue->history=(char **) malloc(n*sizeof(char *));
    for (i=0;i<n;i++){
        *((queue->history)+i)=(char *)malloc ((queue->commandSize)*sizeof(char));
        (*((queue->history)+i))[0]='\0';

    }
    return queue;
}

historyQueue *resize_queue(historyQueue *queue,int n){
    historyQueue *newQueue;
    newQueue=init_queue(n);
    if (queue->tail==-1){
        free (queue);
        return newQueue;
    }
    int start=1,i=0;
    while( start==1 ||(((queue->head +i)%(queue->q_size))!= (queue->tail +1)%(queue->q_size))){
            start=0;
            enqueue(newQueue,*(queue->history+(queue->head +i)%(queue->q_size)));
            i++;
        } 
    free(queue);
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
    queue->no_elem++;
}

void dequeue(historyQueue *queue,char *buf){
    if (buf!=NULL){
        strcpy(buf,*(queue->history + queue->head));
    }
    queue->head=(queue->head+1)%(queue->q_size);
    queue->no_elem--;
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
