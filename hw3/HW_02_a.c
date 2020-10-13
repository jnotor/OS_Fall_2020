#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    // init counter/iterator var
    int arg_count = 0;

    // enter while loop; continue until argv+arg_count is NULL
    // and we've therefore run out of args
    while (*(argv+arg_count)!=NULL){
        // print to terminal the arguement passed into main at
        // position 0 + arg_count
        printf("%s\n", *(argv+arg_count));
        // increment arg_count
        arg_count++;
    }
}