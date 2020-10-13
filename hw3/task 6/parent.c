// Parent creation script
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

// Call tracking variable
static int secondCall = 0;

// Define Signal handler
void SIGINT_signal(int signum){ 
    // Ignore if SIGINT never sent before
    if (secondCall == 0) {
        // Print ignore msg w/specific PID for kid 
        printf("PID %d has recieved a SIGINT that is being ignored\n", getpid());
        // Increment secondCall so that next time, we heed the call
        secondCall++;
    }
    else {
        // Print Honor msg w/specific PID for kid
        printf("PID %d has recieved a SIGINT that is being honored\n", getpid());
        // Child actually exits now
        exit (signum);
    }
}

void launch_kid() {
    // Set signal handler for SIGINT
    signal(SIGINT, SIGINT_signal);

    // Loop indefinitely
    while(1) {}
    exit (0);
}

int main(int argc, char **argv) {
    // "Temp" pid variable used to perform logic between kid and parent
    pid_t fork_returned_pid;

    // Kid PID storage array so we can call them later
    pid_t kid_container[5];
    
    // Iterator variable
    int kid_count;
    // Parent and parent only needs to launch 5 and only 5 children
    for (kid_count=0; kid_count<5; kid_count++) {
        // Attempt to create kid using fork
        fork_returned_pid = fork();

        // Standard check for failed to create kid
        if (fork_returned_pid < 0) {
            printf("The fork() didn't work! Terminate\n");
            exit (0);
        }

        // Change child from parent clone to intended child process
        if (fork_returned_pid == 0) {
            // execvp(*(argv+1), argv+1);
            launch_kid();
            printf("Error converting child to other process\n");
        }
        
        // Store PID of child for future use
        else {
            kid_container[kid_count] = fork_returned_pid;
        }
    }

    // Sleep 5s; then send initial SIGINT to kids
    sleep(5);
    for (kid_count=0; kid_count<5; kid_count++) {
        // Execute Order 66 - KILL with SIGINT
        kill(kid_container[kid_count], SIGINT);
    }

    // Declare status variable    
    int return_status;
    // Sleep another 5s; send SIGINT like you mean it this time
    sleep(5);
    for (kid_count=0; kid_count<5; kid_count++) {
        // Execute Order 66 - KILL with SIGINT
        kill(kid_container[kid_count], SIGINT);
        // Trying to terminate next kid before 1 finished terminating
        // resulted in weird run time errors where the terminal
        // would just indefinitely be waiting for something
        wait(&return_status);
    }

    exit(0);
}

