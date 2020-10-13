// Child creation script
#include <stdlib.h>
#include <sys/types.h>
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

int main() {
    // Set signal handler for SIGINT
    signal(SIGINT, SIGINT_signal);

    // Loop indefinitely
    while(1) {}
    exit (0);
}