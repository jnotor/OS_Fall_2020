#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int isPaused;

void new_stop(int signum){
    if (!isPaused){
        printf("%d is stopped\n", getpid());
        isPaused = 1;
        kill(getpid(), SIGSTOP);
    }
    else {
        isPaused = 0;
        kill(getpid(), SIGCONT);
    }
}

int main() {
    signal(SIGTSTP, new_stop);   // control z

    while(1){
        printf("yes\n");
    }
}