#include <stdio.h>

// add envp pointer to params for main
int main(int argc, char *argv, char **envp) {
    // iterate through the block envp points to until
    // we can no longer (i.e. it points to NULL)
    while(1) {
        char **start = envp;
        while(*start) {
            // print to terminal the current envp val and
            // then increment it
            printf("%s\n", *start++);
            // NOTE: could also just print *envp
            // and then increment on next line
        }
    }

}