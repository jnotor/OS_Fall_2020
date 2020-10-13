#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void forkexample() {
    printf("here");
    if (fork()==0) {
        printf("Hey from kid\n");
    }
    else {
        printf("Hey from mom\n");
    }
}

int main() {
    forkexample();
    // fork();
    // printf("hello\n");
    // fork();
    // fork();
    return 0;
}