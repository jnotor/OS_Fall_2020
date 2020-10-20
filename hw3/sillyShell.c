#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// using this var to resume something paused more than once
static int paused_pid = 0;

/**************************************************************/
/* Text Processing / Parsing Routines                         */
/**************************************************************/

void  parse(char *line, char **argv){
  /* Method to parse input

  param line: pointer to null terminated string; converts whitespace (WS) chars to
              null chars ('\0)
  param argv: array of pointers to arguements; 0th element becomes first non-null
              (token) in line at the 1st position (not 0th). "Last" set to NULL
              marks end
  */

     // record address in the array of address at argv and increment it.  When
     // we run out of tokens in the string, we make the last entry in the array
     // at argv NULL.  This marks the end of pointers to tokens.

    // while loop goes til we run out of characters in the string pointed to by line
     while (*line != '\0') // outer loop.  keep going until the whole string is read
        { // keep moving forward the pointer into the input string until
          // we encounter a non-whitespace character.  While we're at it,
          // turn all those whitespace characters we're seeing into null chars.

          // interweave btwn rewriting WS with '\0' AND "break" at non-WS
          while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '\r')
           { *line = '\0';
             line++;
           }

          // Non-WS found, record in *argv the address for beginning of token
          // I'm pointing at and increment argv
          *argv++ = line;

          // increment the input line pointer until WS
          while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n' && *line !='\r')
               line++;
          }

      // out of chars; mark end of list NULL
     *argv = NULL;
}

/**************************************************************/
/* Signal Handling Routines                                   */
/**************************************************************/
void ignore_signal(int signum){
  /* Function ignores ctrl z and c for parent. handles special case for child
  where we want to pause and continue more than 1x

  @param signum: int value of the signal sent

  NOTE: could be improved by making a dynamic array of paused pids
  so we can pause more than one process at a time but feels outside
  the scope of this project
  */

  // If the user wants to pause
  if (signum == SIGTSTP) {
    // and if the user has paused before, we therefore have a paused_pid recorded
    if (paused_pid) {
      int return_status;
      // call wait on current pid
      waitpid(getpid(), &return_status, WUNTRACED);

      if (WIFSTOPPED(return_status)) {
        printf("\n%d is stopped\n", paused_pid);
      }
      else {
      // wait for specific child to return exit status
        if (WIFEXITED(return_status)) {
          printf("\n%d terminated normally\n", paused_pid);
        }
// reset paused pid since process no longer exists
        paused_pid = 0;
      }
    }
  }
}

/**************************************************************/
/* Shell Command Processing Routines                          */
/**************************************************************/

void execute(char **argv){
  /* Purpose: to execute a non-built in command

    param argv: array of pointers to commands
  */
  pid_t  pid;

  // attempt to fork
  if ((pid = fork()) < 0) {
    printf("*** ERROR: forking child process failed\n");
    exit(1);
  }
  else {
    // child found
    if (pid == 0) {
      execvp(*argv, argv);
      }
    // parent
    else {
      int return_status;
      waitpid(pid, &return_status, WUNTRACED);

      if (WIFSTOPPED(return_status)) {
        printf("\n%d is stopped\n", pid);
        // Doing a little extra here so we don't leave a child paused, since we
        // can only pause one process at a time in the current implementation
        if (paused_pid) {
          printf("trying to kill prev pause\n");
          // kill old child
          kill(paused_pid, SIGKILL);
          // set paused pid to new pid
          paused_pid = pid;
        }
        else {
	    // reset pid since process dead
          paused_pid = pid;
        }
      }
      else {
      // wait for specific child to return exit status
        if (WIFEXITED(return_status)) {
          printf("\n%d terminated normally\n", pid);
        }
      }
    }
  }
}

int main(int argc, char **argv, char **envp) {
  // Initialize vars without initial values
  char cwd[PATH_MAX];
  int job;
  int job_number_int;
  int status;
  pid_t wpid;

  int index = 0;

  // This string will hold the shell prompt string
  char shell_prompt[15];
  // set the default prompt
  strcpy(shell_prompt, "SillyShell");

  // str buffer to hold user input & be parsed; restricts user input to 1024 char
  char  line[1024];

  // unmodified copy of user cmd line input
  char raw_line[1024];

  // pointer to array of 64 pointers to char/strings. Modified after parsing to
  // hold pointers to mem. inside of the string pointed to by pointer line
  char  *line_argv[64];

  signal(SIGINT,  ignore_signal);   // control c
  signal(SIGTSTP, ignore_signal);   // control z

  while (1){
    printf("%s> ",shell_prompt);

    // If no input, print a new line and await further user input
    if(!fgets(line, 1024, stdin)) {
      printf("\n");
      exit(0);
    }

    // set line end to null char
    line[strlen(line)-1]='\0';
    // save a copy of raw_line
    strncpy(raw_line, line, 1024);

    // If something was actually typed, then do something...
    if ((*line != '\0') && (*line > 31)){

      // Parse the input line to break it into token references to addresses
      parse(line, line_argv);

      // Check for Built-In commands
      // Built-in "kill" silly shell commands
      if (strcmp(line_argv[0], "exit")==0 || strcmp(line_argv[0], "done")==0 ||
          strcmp(line_argv[0], "quit")==0 || strcmp(line_argv[0], "q")==0){
        exit(0);
      }
      else {
        // Non-Shell-Terminating Built-In If-Else ladder
        if (strcmp(line_argv[0], "printenv")==0) {
          // set temp start var to envp so we can iterate w/o losing place
          char **start = envp;
          // iterate through the block start until points to NULL
          while(*start) {
            // print to terminal the current envp val & then increment it
            printf("%s\n", *start++);
          }
        }
        else {
          if (strcmp(line_argv[0], "pwd")==0) {
            // check to see we have a current working dir
            if (getcwd(cwd, sizeof(cwd))) {
              // print if so
              printf("%s\n", cwd);
            }
            else {
              printf("getcwd() error\n");
            }
          }
          else {
            if (strcmp(line_argv[0], "cd")==0) {
              if (line_argv[1] != NULL) {
                // NOTE: only excepts absolute paths
                if (chdir(line_argv[1]) == -1) {
                  printf("Requires absolute path to cd\n");
                }
                else {
                  // Have to manaully update env PWD
                  setenv("PWD", line_argv[1], 1);
                }
              }
              else {
                printf("Requires absolute path to cd\n");
              }
            }
            else {
              // Not one of our built-ins; Process the line as if the 1st token is
              // the name of a program in the current path environment & run it
              execute(line_argv);
            }
          } // end cd or exe
        } // end pwd, cd or exe
      } //end printenv, pwd, cd or exe
    } // end if
  } // end while
} // end main
