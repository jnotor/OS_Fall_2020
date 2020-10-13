// Program HW_02_d.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){ 
  printf("here");
  fflush(stdout);
  int test = 0;
  pid_t fork_returned_pid;
  int count;

  fork_returned_pid = fork();

  if (fork_returned_pid < 0)
     { printf("The fork() didn't work! Terminate\n");
       exit (0); }

  for (count = 0; count < 10; count++)
      { if (fork_returned_pid == 0)
            printf("                              child says:  count = %d\n", count);
        else
            printf("parent says: count = %d\n", count);
      }
}

