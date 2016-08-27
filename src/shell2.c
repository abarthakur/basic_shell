#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void  execute(char **argv);
void  parse(char *line, char **argv);



int main(void){

    //dynamically allocate memory instead
    char line[1024];
    int count=0, size, i;
    //dynamically allocate, make configurable
    char *hist[10];

    char  *argv[64];              
    
    //initialize elsewhere
    //introduce "environment"
    //instead initialize to some x
    for(i=0;i<10;i++)
        hist[i]=NULL;


    while(1){
        //add user info here+pwd
        //READ
        printf(">>$:");

        //improve readline. first try using gnu c readline lib
        //then try implementing some of it.
        gets(line);

        //why?
        size=strlen(line);

        //make cleaner , what the hell is happening here?
        //why are we allocating exactly that much space? makes no sense!
        //overhead, completely change this part
        if(hist[count]!=NULL)
            hist[count]=(char*)realloc(hist[count], (size+1)*sizeof(char));
        else        
            hist[count]=(char *)malloc((size+1)*sizeof(char));
        
        //where is the queue implementation of history? 
        //how are we going to be able to rotate? checking for full buffer?
        {//ok, why the curly brackets?
            strcpy(hist[count], line);

            count++;}

        


        printf("\n");
        parse(line, argv); 
        //check if history? change this to check for builtin commands
        //!! and !n

        //implement cd , ls,bg,fg,kill,jobs,alias,wait,exit
        if(strcmp(argv[0], "history")==0)
        {
            for(i=0;i<count;i++)
            {   
                if(hist[i]!=NULL)           
                    printf("%s\n",hist[i]);
            }
            printf("\n");
            continue;
        }

            if (strcmp(argv[0], "exit") == 0)  
                break; //return from shell to main          
            execute(argv);          
    }
    return 0;
}



void  parse(char *line, char **argv)
{   
    
    //ok so basically removes the whitespace?
    //need a better parser!!
    // at the very lowest level , history? piping?
    //what does a parser even do?
     while (*line != '\0') //end of line
    {       
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     
          *argv++ = line;          
          while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') 
               line++;            
     }
     *argv = '\0';                 
}

void  execute(char **argv)
{
    pid_t  pid;
    int    status;
    //give later, store pids.
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







