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
* Lab 1 for course Systemnära Programmering HT20
* Program: Creates a pipeline for commands written from stdin or from a file
*/

int splitString(char *piece, char **args, int indexJump);
void getCommand(int argc, const char **argv);
void runCommand(char *command, char **args, int numArgs, int child, int indexJump);
void waitProcesses(int comCount, pid_t *pids);
void reallocMem(char **com, char **arg, int *argCount, int *indexJumper);
void freeArray(char **arr, int count);
int checkBlankLine(char line[MAX_LENGTH]);


/*
* Main: Runs the code
* Returns: 0
* */
int main(int argc, const char **argv) {

  //Get data from stdio or from a file
  getCommand(argc, argv);

  return 0;
}

void getCommand(int argc, const char **argv) {

  char line[MAX_LENGTH];
  //char *arg1[] = {"", NULL};
  int commandCount = 0;
  int argCount = 0;
  int indexJump = 0;
  //Allocate memory for commands and arguments  
  char **command = (char **) malloc(1024);
  char **args = (char **) malloc(1024);
  int *argCounter = (int *) malloc(10*sizeof(int));
  int *indexJumper = (int *) calloc(10, sizeof(int));

//Check memory allocation, exit if failed
  if(command == NULL) {
    perror("Error allocating memory!");
    exit(EXIT_FAILURE);
  }
  if(args == NULL) {
    perror("Error allocating memory!");
    exit(EXIT_FAILURE);
  }
  if(argCounter == NULL) {
    perror("Error allocating memory!");
    exit(EXIT_FAILURE);
  }
  if(indexJumper == NULL) {
    perror("Error allocating memory!");
    exit(EXIT_FAILURE);
  }

  //If no file, read from stdin
  if(argc == 1) {

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

        if(command == NULL) {
          perror("Error reallocating memory for commands!");
          exit(EXIT_FAILURE);
        }
        if(args == NULL) {
          perror("Error allocating memory!");
          exit(EXIT_FAILURE);
        }

        //Reallocate more memory for arguments if needed
        if(((argCount % 10) == 0)) {
          argCounter = (int *) realloc(argCounter, sizeof(argCounter)*2);
          if( argCounter == NULL) {
            perror("Error reallocating memory!");
            exit(EXIT_FAILURE);
          }  
        }
        if(((indexJump % 10) == 0)) {
          indexJumper = (int *) realloc(argCounter, sizeof(indexJumper)*2);
          if( indexJumper == NULL) {
            perror("Error reallocating memory!");
            exit(EXIT_FAILURE);
          }  
        }

        
        //Check the memory allocation, exit if failed

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
      /*
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
      */
      commandCount++;
      //printf("commandCount: %d\n", commandCount);
      }
    }

    //File stuff
    else if(argc == 2) {

      FILE *in = fopen(argv[1], "r");
      //If there is an error opening file, exit
      if(in == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
      }

      while(fgets(line, MAX_LENGTH, in) != NULL) {
      
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

        //Check the memory allocation, exit if failed
        if(command == NULL) {
          perror("Error reallocating memory for commands!");
          exit(EXIT_FAILURE);
        }
        if(args == NULL) {
          perror("Error allocating memory!");
          exit(EXIT_FAILURE);
        }
        //Reallocate more memory for arguments if needed
        if(((argCount % 10) == 0)) {
          argCounter = (int *) realloc(argCounter, sizeof(argCounter)*2);
          if( argCounter == NULL) {
            perror("Error reallocating memory!");
            exit(EXIT_FAILURE);
          }
        }
        if(((indexJump % 10) == 0)) {
          indexJumper = (int *) realloc(argCounter, sizeof(indexJumper)*2);
          if( indexJumper == NULL) {
            perror("Error reallocating memory!");
            exit(EXIT_FAILURE);
          }  
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
      /*
    
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
      */
      commandCount++;
      //printf("commandCount: %d\n", commandCount);
      }

      //Close file when done
      fclose(in);
          
    }
    //If too many arguments, exit the program
    else {
      fprintf(stderr, "Too many arguments!\n");
      exit(EXIT_FAILURE);
    }

    //pid_t pids[commandCount];  
    pid_t pid;
  
    //fprintf(stderr, "Creating pipes in parent %d\n", getpid());
    //Create enough pipes for all child processes to use
    int pipes[commandCount-1][2];
    for(int i = 0; i < commandCount-1; i++) {
       //Create pipes, exit with error if failure to do so
       if(pipe(pipes[i]) == -1) {
          perror("Error creating pipes!");
          exit(EXIT_FAILURE);
        }

  }
    if(commandCount >= 1) {
  

      //Create the forks
      for(int i = 0; i < commandCount; i++) {
         //If theres a problem forking, report error and exit
         if((pid = fork()) < 0) {
          perror("Error creating fork!");
          exit(EXIT_FAILURE);
         }
         //Child process
         else if(pid == 0) {
           //fprintf(stderr, "created child %d from parent %d\n", getpid(), getppid());
           //Only one child
           if(commandCount == 1) {
             //fprintf(stderr, "Only 1 child... executing: %d\n", commandCount);
             //No redirecting needed, just execute
            fprintf(stderr, "Child %d running command %s\n", getpid(), command[i]);
            runCommand(command[i], args, argCounter[i], i+1, indexJumper[i]);
           }
           //Close relevant pipes
           //First child, only redirect stdout
           if(i == 0) {
              //Close all relevant pipes
              //fprintf(stderr, "Child #%d is first, redirecting output...\n", getpid());
              for(int j = 0; j < commandCount-1; j++) {
              
                if(j != i) {
                  fprintf(stderr, "Child %d closing pipe %d WRITE\n", getpid(), j);
                  close(pipes[j][WRITE_END]);
                }
                fprintf(stderr, "Child %d closing pipe %d READ\n", getpid(), j);
                close(pipes[j][READ_END]);
              }
              //Redirect output to pipe and execute command
              fprintf(stderr, "Child %d redirecting output to pipe %d WRITE\n", getpid(), i);
              if(dup2(pipes[i][WRITE_END], STDOUT_FILENO) < 0){
                perror("Error duplicating file descriptor");
                exit(EXIT_FAILURE);
              }
              //fprintf(stderr, "Running command [%s]...\n");
              //close(pipes[i][WRITE_END]);

              fprintf(stderr, "Child %d running command %s\n", getpid(), command[i]);
              runCommand(command[i], args, argCounter[i], i+1, indexJumper[i]);
              
           }
           //If last child, only redirent stdin
           else if(i == (commandCount-1)) {
             //fprintf(stderr, "Child #%d is last, redirecting input...\n", getpid());
             //Close all relevant pipes
              for(int j = 0; j < commandCount-1; j++) {
                
                if(j != i-1) {
                  fprintf(stderr, "Child %d closing pipe %d READ\n", getpid(), j);
                  close(pipes[j][READ_END]);
                }
                fprintf(stderr, "Child %d closing pipe %d WRITE\n", getpid(), j);
                close(pipes[j][WRITE_END]);
              }
             //dup2(pipes[i][WRITE_END], STDOUT_FILENO);
             //fprintf(stderr, "Child #%d, redirecting input to pipe %d READ...\n", i+1, i-1);
             fprintf(stderr, "Child %d redirecting input to pipe %d READ\n", getpid(), i-1);

             if(dup2(pipes[i-1][READ_END], STDIN_FILENO) < 0 ) {
              perror("Error duplicating file descriptor");
              exit(EXIT_FAILURE);
             }
             //fprintf(stderr, "Child #%d, executing command [%s]...\n", i+1, command[i]);
             //close(pipes[i-1][READ_END]);

             //Executes program
             fprintf(stderr, "Child %d running command %s\n", getpid(), command[i]);
             runCommand(command[i], args, argCounter[i], i+1, indexJumper[i-1]);

           }

           //Otherwise, redirect both input and output
           else {
             //fprintf(stderr, "Child #%d is in between, redirecting input and output...\n", getpid());
             for(int j = 0; j < commandCount-1; j++) {
               
               if(j != (i-1)) {
                fprintf(stderr, "Child %d (middle) closing pipe %d READ\n", getpid(), j);
                close(pipes[j][READ_END]);
               }
               if(j != i) {
                fprintf(stderr, "Child %d (middle) closing pipe %d WRITE\n", getpid(), j);
                close(pipes[j][WRITE_END]);
               }
               
             }
             //fprintf(stderr, "Child #%d, redirecting input to pipe %d READ and output to %d WRITE...\n", i+1, i-1, i);
             fprintf(stderr, "Child %d (middle) redirecting input to pipe %d READ\n", getpid(), i-1);
             fprintf(stderr, "Child %d (middle) redirecting output to pipe %d WRITE\n", getpid(), i);

             if(dup2(pipes[i-1][READ_END], STDIN_FILENO) < 0) {
              perror("Error duplicating file descriptor");
              exit(EXIT_FAILURE);
             }
             if(dup2(pipes[i][WRITE_END], STDOUT_FILENO) < 0) {
              perror("Error duplicating file descriptor");
              exit(EXIT_FAILURE);
             }
             //fprintf(stderr, "Child #%d, executing command [%s]...\n", i+1, command[i]);
             //close(pipes[i][WRITE_END]);
             //close(pipes[i-1][READ_END]);
            fprintf(stderr, "Child %d running command %s\n", getpid(), command[i]);
            runCommand(command[i], args, argCounter[i], i+1, indexJumper[i-1]);

           }
         }


       }

          //Close all pipes in parent process
        for(int i = 0; i < commandCount-1; i++) {
          fprintf(stderr, "Closing pipe %d WRITE / READ in parent %d\n", i, getpid());
          close(pipes[i][READ_END]);
          close(pipes[i][WRITE_END]);
        }

        int status;
        int exitStatus = 0;

        //Wait for all children
        while ((pid=wait(&status)) != -1) {
          fprintf(stderr, "Child %d exited exit status: %d\n", pid, WIFEXITED(status));
          if(WIFEXITED(status) == 0) {
            fprintf(stderr, "child %d exited wrong...\n", pid);
            exitStatus++;
          }
          //exit(EXIT_FAILURE);
          
          //printf("Process %d terminated\n",pid);
        }
        if(exitStatus > 0) {
          fprintf(stderr, "Child exited wrong, exiting %d...\n", exitStatus);
          exit(EXIT_FAILURE);
        }
      }
      //waitProcesses(commandCount, pids);

      //int temp = 0;
      //Wait for all children
      //int status;
      //while ((pid = wait(&status)) > 0);

  

       //fprintf(stderr, "Gone through all processes, 0 left");
       //fprintf(stderr, "Gone through all processes, 0 left");

      //Free memory
      freeArray(args, indexJump);
      freeArray(command, commandCount);
      free(argCounter);
      free(indexJumper);
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
  //Jump to the relevant argument in the array and use that
  else {
    int temp = 0;
    for(int k = indexJump; k <= (indexJump+numArgs)-1; k++) {     
      //fprintf(stderr, "Child %d getting arg from index %d:  %s\n",i+1, k, args[k]);
      arg[temp] = strdup(args[k]);
      temp++;
    }
  }

  arg[numArgs] = NULL;
  //If only one argument, use command as argument
  char *tempArg[] = {command, NULL};

  //Execute command
  if(numArgs == 1) {
    //fprintf(stderr, "Arg counter 0, running program\n");
    if(execvp(command, tempArg) < 0){
      //Exit with error message if failed
      perror("Execv error: ");
      exit(EXIT_FAILURE);
    }
  }
  else {
     if(execvp(command, arg) < 0){
      //Exit with error message if failed
      perror("Execv error: ");
      exit(EXIT_FAILURE);
    }
  }

  //fprintf(stderr, "Executing command %s with args [%s] [%s] [%s]... \n", command[i], arg[0], arg[1], arg[2]);
}


/* Function: splitString
 * Splits a string into certain amounts of arguments, and returns a counter for the amount
 *
 * */
int splitString(char *piece, char **args, int indexJump) {
    //printf("Adjusting insertion by %d indexes..\n", indexJump);
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
/*
* waitProcesses
* Makes parent process wait for each child until it completes and checks its exit status, if a child has exited abnormaly, exit
* */
void waitProcesses(int comCount, pid_t *pids) {
      int status;
      int temp = 0;

    //Wait for all children
     for(int i = 0; i < comCount; i++) {
       //fprintf(stderr, "Waiting for child %d to exit...\n", pids[temp]);
        if(waitpid(pids[temp], &status, 0) != -1) {
          if(WIFEXITED(status)) {
           fprintf(stderr, "child %d exits normally with status %d..\n", pids[temp], WIFEXITED(status));
           continue;
         }
         else {
           fprintf(stderr, "child %d exited abnormaly with status %d\n", pids[temp], WIFEXITED(status));
           exit(EXIT_FAILURE);
         }
        }
        else {
          perror("Wait failed!");
          exit(EXIT_FAILURE);
        }
         //fprintf(stderr, "Parent signing off, child exited with status %d \n", status);
         //Check child processes and if they exited incorrectly, exit with error
         temp++;
       }
       //fprintf(stderr, "Gone through all processes, 0 left");
       
}
/*
* freeArray
* Takes an array and goes through to free the indexes aswell as the array itself
* */
void freeArray(char **arr, int count) {
  //Go through each index and free
  for(int i = 0; i < count; i++) {
    free(arr[i]);
  }
  //Free the array itself
  free(arr);
}
