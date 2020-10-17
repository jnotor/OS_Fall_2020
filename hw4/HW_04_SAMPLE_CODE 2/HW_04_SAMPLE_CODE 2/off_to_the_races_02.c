#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int c = 0;

pthread_mutex_t lock;


void *fnC()
{
    int i;
    for(i=0;i<10;i++)
    {   //pthread_mutex_lock(&lock);
        c++;
        printf(" %d", c); 
        //pthread_mutex_unlock(&lock);
    }   
       
}


int main()
{
    int rt1, rt2;
    pthread_t t1, t2; 
    int trial_count = 0;

    pthread_mutex_init(&lock, NULL);
    
    for (trial_count = 0; trial_count < 1000; trial_count++)
{
    c = 0;
    /* Create two threads */
    if( (rt1=pthread_create( &t1, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt1);
    if( (rt2=pthread_create( &t2, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt2);
    /* Wait for both threads to finish */
    pthread_join( t1, NULL);
    pthread_join( t2, NULL);
    printf ("\n");
}

    return 0;

}
