#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

#define READ_END  0
#define WRITE_END 1
#define MAX_LENGTH 1024

/*
* Author: Marcus Karlström
* CS-user: c13mkm
* Laboration 1 för Systemnära Programmering
* Program: Skapar en pipeline för kommandon antingen från stdin eller från en fil
*/


int main(int argc, const char **argv) {

  //int fd[2];
  int indexJump = 0;
  //int commandCount = 0;

  //If no file, read from stdin
  if(argc == 1) {
    
    //FILE *in;
    char line[MAX_LENGTH];
    //char *arg1[] = {"", NULL};
    int commandCount = 0;
    int argCount = 0;
    //Allocate memory for commands and arguments  
    char **command = (char **) malloc(1024);
    char **args = (char **) malloc(1024);
    int *argCounter = (int *) malloc(100*sizeof(int));
    int *indexJumper = (int *) calloc(100, sizeof(int));

    //Check memory allocation, exit if failed
    if(command == NULL) {
      perror("Error allocating memory!");
      exit(0);
    }
    if(args == NULL) {
      perror("Error allocating memory!");
      exit(0);
    }
    //Read data from stdin
    while(fgets(line, MAX_LENGTH, stdin) != NULL) {
      
      
      //Remove newline character from fgets
      size_t len = strlen(line);
      if (len > 0 && line[len-1] == '\n') {
        line[--len] = '\0';
      }

      if(commandCount >= 1) {
        //Allocate new memory for each new command
        command = (char **) realloc(command, (sizeof(command) + sizeof(char)*1024));
        args = (char **) realloc(args, (sizeof(args) + sizeof(char)*1024));
        if(argCount == 100) {
          //Reallocate more memory for arguments
          argCounter = (int *) realloc(argCounter, sizeof(argCounter)*2);
        }
        //Need command counter to know, use variable and while loop (???)
        
                  

        //Check the memory allocation, exit if failed

        if(command == NULL) {
          perror("Error reallocating memory for commands!\n");
          exit(1);
        }
        if(args == NULL) {
          perror("Error allocating memory!");
          exit(1);
        }
        
        //command[commandCount] = strdup(line);
        int argCount = 0;
        char *piece = strtok(line, " ");
        printf("Adding %s to commands array... \n", piece);
        command[commandCount] = strdup(piece);
        //Add 
        printf("Adjusting insertion by %d indexes..\n", indexJump);
        int j = indexJump;

        while(piece != NULL) {

          args[j] = strdup(piece);
          piece = strtok(NULL, " ");
                       
          argCount++;
          j++;
        }

        indexJumper[commandCount] = indexJump + argCount;
        indexJump = indexJump + argCount;
        argCounter[commandCount] = argCount;
        printf("Arg count for command [%s]: %d\n", command[commandCount], argCounter[commandCount]);
        
      }
      
      else {
        //Check amount of arguments for each command
        int argCount = 0;
        char *piece = strtok(line, " ");
        printf("Adding %s to commands array... \n", piece);
        command[commandCount] = strdup(piece);
        printf("Adjusting insertion by %d indexes..\n", indexJump);
        int j = indexJump;

        while(piece != NULL) {
                          
          //printf("%s\n", piece);
          //Store arguments
          //printf("Adding %s to args arrayindex %d... \n", piece, j);
          /* REMOVES COMMAND FROM FIRST ARG VALUE
          if(strcmp(piece, command[commandCount]) != 0) {
            args[commandCount][j-1] = strdup(piece);
          }*/

          args[j] = strdup(piece);
          piece = strtok(NULL, " ");
                       
          argCount++;   
          j++;
       }
       argCounter[commandCount] = argCount;
       indexJumper[commandCount] = indexJump + argCount;
       indexJump = indexJump + argCount;
       printf("Arg count for command [%s]: %d\n", command[commandCount], argCounter[commandCount]);

      }
            printf("indexJumper: ");
            for(int b = 0; b < 10; b++) {
                  printf("[%d]", indexJumper[b]);
            }
            printf("\n");
           printf("argCounter: ");
            for(int d = 0; d < 10; d++) {
                  printf("[%d]", argCounter[d]);
            }
            printf("\n");

            printf("Command array: ");
             for (int x = 0; x < 10; x++) {
                  printf("[%s]", command[x]);
            }
            printf("\n");

            printf("Args array: ");
            for (int k = 0; k < 10; k++) {
                  printf("[%s]", args[k]);
            }
            printf("\n");

      commandCount++;
      printf("commandCount: %d\n", commandCount);
    }

    //Create enough pipes for all child processes to use
    int pipes[commandCount-1][2];

    
    for(int i = 0; i < commandCount-1; i++) {
        if(pipe(pipes[i]) == -1) {
          perror("Error creating pipes!");
          exit(EXIT_FAILURE);
        }
        else{
          printf("Pipe created!\n");
        }
    }

    if(commandCount >= 1) {
      pid_t pids[commandCount];  
      int processes = commandCount;
      //Create child proc
      //Redirect input/output
      //Execute command
      //Child process should use pipes[i][0] for reading and pipes[i+1][1] for writing?
      //Last process should use pipes[i][0] for reading only

      //Create the forks
      for(int i = 0; i < commandCount; i++) {
         //If theres a problem forking, report error and exit
         if((pids[i] = fork()) == -1) {
          perror("Error creating fork!");
          exit(EXIT_FAILURE);
         }
         //Child process
         else if(pids[i] == 0) {
           //Close relevant pipes
           //fprintf(stderr, "Child %d created from parent %d\n", getpid(), getppid());
           //First child, only redirect stdout
           if(i == 0) {
              //fprintf(stderr, "Child #%d, redirecting output...\n", i+1);
              //Close all relevant pipes
              for(int j = 0; j < commandCount-1; j++) {
              
                if(j != i) {
                  //fprintf(stderr, "Child %d closing pipe %d WRITE\n", i+1, j);
                  close(pipes[j][WRITE_END]);
                }
                //fprintf(stderr, "Child %d closing pipe %d READ\n", i+1, j);
                close(pipes[j][READ_END]);
              }
              //Redirect output to pipe and execute command
              //fprintf(stderr, "Child #%d, redirecting output to pipe %d WRITE...\n", i+1, i);
              dup2(pipes[i][WRITE_END], STDOUT_FILENO);
              //fprintf(stderr, "Child #%d, executing command [%s] with argument [%s]...\n", i+1, command[i], args[i]);

              
              //Get arguments for command
              char *arg[argCounter[i]];
              
              fprintf(stderr, "i: %d\n", i);
              for(int k = 0; k < argCounter[i]; k++) {
                fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
                arg[k] = strdup(args[k]);
                
              }
              arg[argCounter[i]] = NULL;
              char *tempArg[] = {"", NULL};
              if(argCounter[i] == 1) {
                fprintf(stderr, "Arg counter 0, running program\n");
                execvp(command[i], tempArg);
              }
              //Execute command
              fprintf(stderr, "Executing command %s with args [%s] [%s] [%s]... \n", command[i], arg[0], arg[1], arg[2]);
              execvp(command[i], arg);
              
              
           }
           //If last child, only redirent stdin
           else if(i == (commandCount-1)) {
             //fprintf(stderr, "Child #%d, redirecting input...\n", commandCount);
             //Close all relevant pipes
              for(int j = 0; j < commandCount-1; j++) {
                
                if(j != i-1) {
                  //fprintf(stderr, "Child %d closing pipe %d READ\n", i+1, j);
                  close(pipes[j][READ_END]);
                }
                //fprintf(stderr, "Child %d closing pipe %d WRITE\n", i+1, j);
                close(pipes[j][WRITE_END]);
              }
             //dup2(pipes[i][WRITE_END], STDOUT_FILENO);
             //fprintf(stderr, "Child #%d, redirecting input to pipe %d READ...\n", i+1, i-1);
             dup2(pipes[i-1][READ_END], STDIN_FILENO);
             //fprintf(stderr, "Child #%d, executing command [%s]...\n", i+1, command[i]);
             

             char *arg[argCounter[i]];
             int temp = 0;
             for(int k = indexJumper[i-1]; k <= (indexJumper[i-1]+argCounter[i])-1; k++) {
               
               fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
                arg[temp] = strdup(args[k]);
                temp++;
              }
              fprintf(stderr, "Executing command %s with args [%s]... \n", command[i], arg[0]);
              arg[argCounter[i]] = NULL;
              char *tempArg[] = {"", NULL};
              if(argCounter[i] == 1) {
                fprintf(stderr, "Arg counter 0, running program\n");
                execvp(command[i], tempArg);
              }
              //Execute command
              
              execvp(command[i], arg);
            }

           //Otherwise, redirect both input and output
           else {
             //fprintf(stderr, "Child #%d, redirecting input and output...\n", i+1);
             for(int j = 0; j < commandCount-1; j++) {
               
               if(j != (i-1)) {
                fprintf(stderr, "Child %d closing pipe %d READ\n", i+1, j);
                close(pipes[j][READ_END]);
               }
               if(j != i) {
                 fprintf(stderr, "Child %d closing pipe %d WRITE\n", i+1, j);
                close(pipes[j][WRITE_END]);
               }
               
             }
             //fprintf(stderr, "Child #%d, redirecting input to pipe %d READ and output to %d WRITE...\n", i+1, i-1, i);
             dup2(pipes[i-1][READ_END], STDIN_FILENO);
             dup2(pipes[i][WRITE_END], STDOUT_FILENO);
             fprintf(stderr, "Child #%d, executing command [%s]...\n", i+1, command[i]);

             
              char *arg[argCounter[i]];
              int temp = 0;
              for(int k = indexJumper[i-1]; k <= (indexJumper[i-1]+argCounter[i])-1; k++) {

                fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
                arg[temp] = strdup(args[k]);
                temp++;
              }
              arg[argCounter[i]] = NULL;
              char *tempArg[] = {"", NULL};
              if(argCounter[i] == 1) {
                fprintf(stderr, "Arg counter 0, running program\n");
                execvp(command[i], tempArg);
              }
               //Execute command
               //fprintf(stderr, "Executing command %s with args [%s] [%s] [%s]... \n", command[i], arg[0], arg[1], arg[2]);
              execvp(command[i], arg);
           }
           
         }

       }

      //Wait for children
      //Close all pipes
      for(int i = 0; i < commandCount-1; i++) {
        close(pipes[i][READ_END]);
        close(pipes[i][WRITE_END]);
      }
      //Wait for children to complete commands
      int status;
      pid_t temp_pid;
      while(processes > 0 ){
         temp_pid = wait(&status);
         printf("Child with PID %ld exited with status 0x%x. \n", (long)temp_pid, status);
         processes--;
       }
      }

    //After use, free allocated memory
    for(int i = 0; i < commandCount; i++) {
      free(command[i]);
    }
    for(int j = 0; j < commandCount; j++) {
      free(args[j]);
    }
    
    free(command);
    free(args);
    free(argCounter);
  } 

  //If theres a file, read from it
  else if(argc == 2) {
    FILE *in;
    char line[MAX_LENGTH];
    //char *arg1[] = {"", NULL};
    int commandCount = 0;
    int argCount = 0;
    //Allocate memory for commands and arguments  
    char **command = (char **) malloc(1024);
    char **args = (char **) malloc(1024);
    int *argCounter = (int *) malloc(100*sizeof(int));
    int *indexJumper = (int *) calloc(100, sizeof(int));

    //Check memory allocation, exit if failed
    if(command == NULL) {
      perror("Error allocating memory!");
      exit(0);
    }
    if(args == NULL) {
      perror("Error allocating memory!");
      exit(0);
    }

    //Open the file for reading, or print error and exit if it fails
    if(fopen(argv[1], "r") == NULL) {
          perror(argv[1]);
          exit(EXIT_FAILURE);
    }
    in = fopen(argv[1], "r");
    
    //Read data from stdin
    while(fgets(line, MAX_LENGTH, in) != NULL) {
      
      
      //Remove newline character from fgets
      
      size_t len = strlen(line);
      if (len > 0 && line[len-1] == '\n') {
        line[--len] = '\0';
      }
      

      if(commandCount >= 1) {
        //Allocate new memory for each new command
        command = (char **) realloc(command, (sizeof(command) + sizeof(char)*1024));
        args = (char **) realloc(args, (sizeof(args) + sizeof(char)*1024));
        if(argCount == 100) {
          //Reallocate more memory for arguments
          argCounter = (int *) realloc(argCounter, sizeof(argCounter)*2);
        }
        //Need command counter to know, use variable and while loop (???)
        
                  

        //Check the memory allocation, exit if failed

        if(command == NULL) {
          perror("Error reallocating memory for commands!\n");
          exit(1);
        }
        if(args == NULL) {
          perror("Error allocating memory!");
          exit(1);
        }
        
        //command[commandCount] = strdup(line);
        int argCount = 0;
        char *piece = strtok(line, " ");
        printf("Adding %s to commands array... \n", piece);
        command[commandCount] = strdup(piece);
        //Add 
        printf("Adjusting insertion by %d indexes..\n", indexJump);
        int j = indexJump;

        while(piece != NULL) {

          args[j] = strdup(piece);
          piece = strtok(NULL, " ");
                       
          argCount++;
          j++;
        }

        indexJumper[commandCount] = indexJump + argCount;
        indexJump = indexJump + argCount;
        argCounter[commandCount] = argCount;
        printf("Arg count for command [%s]: %d\n", command[commandCount], argCounter[commandCount]);
        
      }
      
      else {
        //Check amount of arguments for each command
        int argCount = 0;
        char *piece = strtok(line, " ");
        printf("Adding %s to commands array... \n", piece);
        command[commandCount] = strdup(piece);
        printf("Adjusting insertion by %d indexes..\n", indexJump);
        int j = indexJump;

        while(piece != NULL) {
                          
          //printf("%s\n", piece);
          //Store arguments
          //printf("Adding %s to args arrayindex %d... \n", piece, j);
          /* REMOVES COMMAND FROM FIRST ARG VALUE
          if(strcmp(piece, command[commandCount]) != 0) {
            args[commandCount][j-1] = strdup(piece);
          }*/

          args[j] = strdup(piece);
          piece = strtok(NULL, " ");
                       
          argCount++;   
          j++;
       }
       argCounter[commandCount] = argCount;
       indexJumper[commandCount] = indexJump + argCount;
       indexJump = indexJump + argCount;
       printf("Arg count for command [%s]: %d\n", command[commandCount], argCounter[commandCount]);

      }
            printf("indexJumper: ");
            for(int b = 0; b < 10; b++) {
                  printf("[%d]", indexJumper[b]);
            }
            printf("\n");
           printf("argCounter: ");
            for(int d = 0; d < 10; d++) {
                  printf("[%d]", argCounter[d]);
            }
            printf("\n");

            printf("Command array: ");
             for (int x = 0; x < 10; x++) {
                  printf("[%s]", command[x]);
            }
            printf("\n");

            printf("Args array: ");
            for (int k = 0; k < 10; k++) {
                  printf("[%s]", args[k]);
            }
            printf("\n");
 
      commandCount++;
      printf("commandCount: %d\n", commandCount);
    }

    //Create enough pipes for all child processes to use
    int pipes[commandCount-1][2];

    
    for(int i = 0; i < commandCount-1; i++) {
        if(pipe(pipes[i]) == -1) {
          perror("Error creating pipes!");
          exit(EXIT_FAILURE);
        }
        else{
          printf("Pipe created!\n");
        }
    }

    if(commandCount >= 1) {
      pid_t pids[commandCount];  
      int processes = commandCount;
      //Create child proc
      //Redirect input/output
      //Execute command
      //Child process should use pipes[i][0] for reading and pipes[i+1][1] for writing?
      //Last process should use pipes[i][0] for reading only

      //Create the forks
      for(int i = 0; i < commandCount; i++) {
         //If theres a problem forking, report error and exit
         if((pids[i] = fork()) == -1) {
          perror("Error creating fork!");
          exit(EXIT_FAILURE);
         }
         //Child process
         else if(pids[i] == 0) {
           //Close relevant pipes
           //fprintf(stderr, "Child %d created from parent %d\n", getpid(), getppid());
           //First child, only redirect stdout
           if(i == 0) {
              //fprintf(stderr, "Child #%d, redirecting output...\n", i+1);
              //Close all relevant pipes
              for(int j = 0; j < commandCount-1; j++) {
              
                if(j != i) {
                  //fprintf(stderr, "Child %d closing pipe %d WRITE\n", i+1, j);
                  close(pipes[j][WRITE_END]);
                }
                //fprintf(stderr, "Child %d closing pipe %d READ\n", i+1, j);
                close(pipes[j][READ_END]);
              }
              //Redirect output to pipe and execute command
              //fprintf(stderr, "Child #%d, redirecting output to pipe %d WRITE...\n", i+1, i);
              dup2(pipes[i][WRITE_END], STDOUT_FILENO);
              //fprintf(stderr, "Child #%d, executing command [%s] with argument [%s]...\n", i+1, command[i], args[i]);

              
              //Get arguments for command
              char *arg[argCounter[i]];
              
              fprintf(stderr, "i: %d\n", i);
              for(int k = 0; k < argCounter[i]; k++) {
                fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
                arg[k] = strdup(args[k]);
                
              }
              arg[argCounter[i]] = NULL;
              
              //Execute command
              fprintf(stderr, "Executing command %s with args [%s] [%s] [%s]... \n", command[i], arg[0], arg[1], arg[2]);
              execvp(command[i], arg);
              
              
           }
           //If last child, only redirent stdin
           else if(i == (commandCount-1)) {
             //fprintf(stderr, "Child #%d, redirecting input...\n", commandCount);
             //Close all relevant pipes
              for(int j = 0; j < commandCount-1; j++) {
                
                if(j != i-1) {
                  //fprintf(stderr, "Child %d closing pipe %d READ\n", i+1, j);
                  close(pipes[j][READ_END]);
                }
                //fprintf(stderr, "Child %d closing pipe %d WRITE\n", i+1, j);
                close(pipes[j][WRITE_END]);
              }
             //dup2(pipes[i][WRITE_END], STDOUT_FILENO);
             //fprintf(stderr, "Child #%d, redirecting input to pipe %d READ...\n", i+1, i-1);
             dup2(pipes[i-1][READ_END], STDIN_FILENO);
             //fprintf(stderr, "Child #%d, executing command [%s]...\n", i+1, command[i]);
             

             char *arg[argCounter[i]];
             int temp = 0;
             for(int k = indexJumper[i-1]; k <= (indexJumper[i-1]+argCounter[i])-1; k++) {
               
               fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
                arg[temp] = strdup(args[k]);
                temp++;
              }
              fprintf(stderr, "Executing command %s with args [%s]... \n", command[i], arg[0]);
              arg[argCounter[i]] = NULL;
              //Execute command
              
              execvp(command[i], arg);
            }

           //Otherwise, redirect both input and output
           else {
             //fprintf(stderr, "Child #%d, redirecting input and output...\n", i+1);
             for(int j = 0; j < commandCount-1; j++) {
               
               if(j != (i-1)) {
                fprintf(stderr, "Child %d closing pipe %d READ\n", i+1, j);
                close(pipes[j][READ_END]);
               }
               if(j != i) {
                 fprintf(stderr, "Child %d closing pipe %d WRITE\n", i+1, j);
                close(pipes[j][WRITE_END]);
               }
               
             }
             //fprintf(stderr, "Child #%d, redirecting input to pipe %d READ and output to %d WRITE...\n", i+1, i-1, i);
             dup2(pipes[i-1][READ_END], STDIN_FILENO);
             dup2(pipes[i][WRITE_END], STDOUT_FILENO);
             fprintf(stderr, "Child #%d, executing command [%s]...\n", i+1, command[i]);

             
              char *arg[argCounter[i]];
              int temp = 0;
              for(int k = indexJumper[i-1]; k <= (indexJumper[i-1]+argCounter[i])-1; k++) {

                fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
                arg[temp] = strdup(args[k]);
                temp++;
              }
              arg[argCounter[i]] = NULL;
               //Execute command
               //fprintf(stderr, "Executing command %s with args [%s] [%s] [%s]... \n", command[i], arg[0], arg[1], arg[2]);
              execvp(command[i], arg);
           }
           
         }

       }

      //Wait for children
      //Close all pipes
      for(int i = 0; i < commandCount-1; i++) {
        close(pipes[i][READ_END]);
        close(pipes[i][WRITE_END]);
      }
      //Wait for children to complete commands
      int status;
      pid_t temp_pid;
      while(processes > 0 ){
         temp_pid = wait(&status);
         printf("Child with PID %ld exited with status 0x%x. \n", (long)temp_pid, status);
         processes--;
       }
      }

    //After use, free allocated memory
    for(int i = 0; i < commandCount; i++) {
      free(command[i]);
    }
    for(int j = 0; j < commandCount; j++) {
      free(args[j]);
    }
    

    //Free memory and close file
    free(command);
    free(args);
    free(argCounter);
    fclose(in);
      
  }
  //Else, print error and exit
  else {
      printf("Error, something went wrong!\n");
      exit(1);
    }


  return 0;
}
