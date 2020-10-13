#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    printf("How many kids you want? ");

    // Declare user input variable
    int kids = 0;
    // Take in user input at kids
    scanf("%d", &kids);

    // Declare iterator
    int iterator;
    // Create as many kids as user specified input via for loop
    for (iterator=0; iterator<kids;iterator++) {
        // If the fork call returns 0, we're in the kid/clone process
        if (fork() == 0) {
            // Print hello message + child PID
            printf("Hello from child process %d\n", getpid());
            // Kill child so we dont have child create more children
            exit(0);
        }
        else {
            // Wait if parent
            wait(NULL);
        }
    }
    // Print parent PID; Children will never reach this line
    printf("Hello from parent process %d\n", getpid());
}
