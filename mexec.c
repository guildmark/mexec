#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <ctype.h>

#define READ_END  0
#define WRITE_END 1
#define MAX_LENGTH 1024

/*
* Author: Marcus Karlström
* CS-user: c13mkm
* Lab 1 for course Systemnära Programmering
* Program: Skapar en pipeline för kommandon antingen från stdin eller från en fil
*/

int splitString(char *piece, char **args, int indexJump);
void runCommand(char *command, char **args, int numArgs, int child, int indexJump);
void freeArray(char **arr, int count);
int checkBlankLine(char line[MAX_LENGTH]);
//void freeMemory(char **args, char **com, int comCount, int *argCounter, int *indexJumper);
//void createPipes(int const **pipes, int comCount);


int main(int argc, const char **argv) {

  int indexJump = 0;

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
      
      //Check for blank lines
      if(checkBlankLine(line)) {
        fprintf(stderr, "No blank lines are permitted!\n");
        exit(EXIT_FAILURE);
      }

      //Remove newline character from fgets
      size_t len = strlen(line);
      if (len > 0 && line[len-1] == '\n') {
        line[--len] = '\0';
      }

      if(commandCount >= 1) {
        //Allocate new memory for each new command
        command = (char **) realloc(command, (sizeof(command) + sizeof(char)*1024));
        args = (char **) realloc(args, (sizeof(args) + sizeof(char)*1024));
        //Reallocate more memory for arguments if needed
        if(((argCount % 100) == 0)) {
          argCounter = (int *) realloc(argCounter, sizeof(argCounter)*2);
          if( argCounter == NULL) {
            perror("Error reallocating memory!");
            exit(EXIT_FAILURE);
          }
        }
        //Need command counter to know, use variable and while loop (???)
        
                  

        //Check the memory allocation, exit if failed

        if(command == NULL) {
          perror("Error reallocating memory for commands!");
          exit(1);
        }
        if(args == NULL) {
          perror("Error allocating memory!");
          exit(1);
        }
        //Split the commands and arguments
        char *piece = strtok(line, " ");
        command[commandCount] = strdup(piece);
        //Split rest of arguments and keep track of indexes
        argCount = splitString(piece, args, indexJump);

        indexJumper[commandCount] = indexJump + argCount;
        indexJump = indexJump + argCount;
        argCounter[commandCount] = argCount;
      }
      
      else {
        //Split initial command
        char *piece = strtok(line, " ");
        command[commandCount] = strdup(piece);
        
        //Split rest of arguments and keep track of indexes and arguments
        argCount = splitString(piece, args, indexJump);

        indexJumper[commandCount] = indexJump + argCount;
        indexJump = indexJump + argCount;
        argCounter[commandCount] = argCount;
      }

      //TEST PRINTS - DELETE LATER
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
        /*
        else{
          printf("Pipe created!\n");
        }
        */
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
           //First child, only redirect stdout
           if(i == 0) {
              //Close all relevant pipes
              for(int j = 0; j < commandCount-1; j++) {
              
                if(j != i) {
                  close(pipes[j][WRITE_END]);
                }
                close(pipes[j][READ_END]);
              }
              //Redirect output to pipe and execute command
              dup2(pipes[i][WRITE_END], STDOUT_FILENO);

              runCommand(command[i], args, argCounter[i], i+1, indexJumper[i]);
              
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
             
             //Executes program
             runCommand(command[i], args, argCounter[i], i+1, indexJumper[i-1]);

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

             runCommand(command[i], args, argCounter[i], i+1, indexJumper[i-1]);

           }
         }

       }

      //Wait for children
      //Close all pipes
      for(int i = 0; i < commandCount-1; i++) {
        close(pipes[i][READ_END]);
        close(pipes[i][WRITE_END]);
      }
      
      int status;
      pid_t temp_pid;
      //Wait for children to complete commands
      while(processes > 0 ){
         temp_pid = wait(&status);
         printf("Child with PID %ld exited with status 0x%x. \n", (long)temp_pid, status);
         processes--;
       }
      }

      printf("Freeing %d indexes from arg... \n", argCount);
      freeArray(args, indexJump);
      freeArray(command, commandCount);
      free(argCounter);
      free(indexJumper);
  }
  else if(argv == NULL) {
    printf("TEST\n");
  }
}

/* Function: splitString
 * Splits a string into certain amounts of arguments, and returns a counter for the amount
 *
 * */
int splitString(char *piece, char **args, int indexJump) {
    printf("Adjusting insertion by %d indexes..\n", indexJump);
    int tempCount = 0;
    int j = indexJump;
    //
     while(piece != NULL) {
          args[j] = strdup(piece);
          piece = strtok(NULL, " ");        
          tempCount++;   
          j++;
       }

    return tempCount;
}

/*
* Function: runCommand
* Execute command with arguments and checks to see what child process is in use
* */
void runCommand(char *command, char **args, int numArgs, int child, int indexJump) {

  char *arg[numArgs];
                
  if(child == 1) {
    for(int k = 0; k < numArgs; k++) {
      //fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
      arg[k] = strdup(args[k]);
    }
  }
  else {
    int temp = 0;
    for(int k = indexJump; k <= (indexJump+numArgs)-1; k++) {     
      //fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
      arg[temp] = strdup(args[k]);
      temp++;
    }
  }

  arg[numArgs] = NULL;
  char *tempArg[] = {"", NULL};
  if(numArgs == 1) {
    //fprintf(stderr, "Arg counter 0, running program\n");
    execvp(command, tempArg);
  }
  //Execute command
  //fprintf(stderr, "Executing command %s with args [%s] [%s] [%s]... \n", command[i], arg[0], arg[1], arg[2]);
  execvp(command, arg);
}

/*
* checkBlankLine
* Checks for blank lines by looking for characters
* Returns: 0 if any character is not space, otherwise 1
* */
int checkBlankLine(char line[MAX_LENGTH]) {
  while(*line) {
    if(!isspace(*line++)) {
      return 0;
    }
  }
  return 1;
}

void freeArray(char **arr, int count) {
   for(int i = 0; i < count; i++) {
      free(arr[i]);
    }
    free(arr);
}


/*
void freeMemory(char **args, char **com, int comCount, int *argCounter, int *indexJumper) {

    printf("Freeing memory...\n");
      //After use, free allocated memory
    for(int i = 0; i < comCount; i++) {
      free(com[i]);
    }
    for(int j = 0; j < comCount; j++) {
      free(args[j]);
    }
    free(com);
    free(args);
    free(argCounter);
    free(indexJumper);
    printf("Memory freed...\n");
}
*/


/*
void createPipes(int const **pipes, int comCount) {
  for(int i = 0; i < comCount-1; i++) {
       if(pipe(pipes[i]) == -1) {
          perror("Error creating pipes!");
          exit(EXIT_FAILURE);
        }
        else{
          printf("Pipe created!\n");
        }
  }
}
*/