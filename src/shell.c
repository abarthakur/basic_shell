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
    
int  main(void)
{
     char  line[1024];             
     char  *argv[64];              
     char *hist[10];
     int count=0, size, i;
     for(i=0;i<10;i++)
        hist[i]=NULL;
     while (1)
     {                  
          printf("New Line-> ");     
          gets(line);
          //if(strcmp(line, "\n")){
                
            //  continue;}
      size=strlen(line);
      if(hist[count]!=NULL)
            hist[count]=(char*)realloc(hist[count], (size+1)*sizeof(char));
      else        
            hist[count]=(char *)malloc((size+1)*sizeof(char));
      
      {strcpy(hist[count], line);
      count++;}             
          printf("\n");
          parse(line, argv); 
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
     while (*line != '\0') 
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
     pid = fork();
     if (pid < 0) 
    {     
          printf("ERROR:-2\n");
          return;
    }
     else if (pid == 0)
     {          
          if (execvp(argv[0], argv) < 0)
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

