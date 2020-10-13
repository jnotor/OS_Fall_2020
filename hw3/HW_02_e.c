// Program HW_02_e.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)

{ pid_t fork_returned_pid;
  int count;

  fork_returned_pid = fork();

  if (fork_returned_pid < 0)
     { printf("The fork() didn't work! Terminate\n");
       exit (0); }

  if (fork_returned_pid != 0) wait(NULL);   // This line is new

  for (count = 0; count < 1000; count++)
      { if (fork_returned_pid == 0) {
            printf("                              child says:  count = %d\n", count);
            wait(NULL);
      }
        else {
            printf("parent says: count = %d\n", count);
        }
      }
}

