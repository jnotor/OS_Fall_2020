/**This program implements the standard dining philosophers problem as described
 *  in the book and in the Wikipedia article. Specifically, in THIS version, a
 * hungry philosopher will attempt to get the chopsticks immediately to the left
 *  and right of the seating position. Each philosopher is implemented as a
 * separate pthread. This version also has a “finite number of meals”. All threads
 *  will end when that number of meals have been consumed. In the default version
 *  of the code I’m giving you, there are 10 philosophers and 10 million meals
 * available. As there is no deadlock protection. This code will DEFINITELY
 * deadlock before all 10 million meals are consumed.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Num philosophers
#define PHILOSOPHER_COUNT 10

// Num meals; overall process ends (threads terminate) when all consumed
// NOTE: It may be possible that not all philosophers get to eat the same amount
#define AVAILABLE_MEALS 10000000


// Each philosopher is represented by 1 "think/start eating/finish eating" thread.
pthread_t philosopher[PHILOSOPHER_COUNT];

// Num chopstick_mutexes = num philosophers
pthread_mutex_t chopstick[PHILOSOPHER_COUNT];

// global tracking var for num meals eaten; all gone when == 0
int MEALS_ON_TABLE = AVAILABLE_MEALS;

// Meals counter mutex to prevent race conditions
pthread_mutex_t MEAL_COUNT_LOCK;

// declare waiter mutex
pthread_mutex_t arbiter_waiter;

/** Method to create philosopher eat/think process where a given philosopher, who
 * knows his own number, grabs the chopsticks to her/his right and left.  The
 * chopstick to the left of philosopher N is chopstick N.  The chopstick to the
 * right of philosopher N is chopstick N+1
 *
 * @param philosopher_number_ptr: ptr to the number of the philosopher
 */
void *philosopher_program(void *philosopher_number_ptr) {
   int philosopher_number = *((int *) philosopher_number_ptr);
   free(philosopher_number_ptr);

   int how_many_times_did_I_eat = 0;

   while (MEALS_ON_TABLE > 0) {
      usleep(1);

      // get waiter attention/lock
      pthread_mutex_lock(&arbiter_waiter);

      // grab/lock chopsticks
      pthread_mutex_lock(&chopstick[philosopher_number]);
      pthread_mutex_lock(&chopstick[(philosopher_number + 1) % PHILOSOPHER_COUNT]);

      // eating
      usleep(2);

      // done eating. put chopsticks back
      if (MEALS_ON_TABLE > 0) {
         // Increment thread access count
         how_many_times_did_I_eat++;

         // lock meal count mutex
         pthread_mutex_lock(&MEAL_COUNT_LOCK);
         // Decrement number of meals available
         MEALS_ON_TABLE--;


         // unlock meal count mutex
         pthread_mutex_unlock(&MEAL_COUNT_LOCK);
      }

      // put down/unlock chopsticks
      pthread_mutex_unlock(&chopstick[philosopher_number]);
      pthread_mutex_unlock(&chopstick[(philosopher_number+1)%PHILOSOPHER_COUNT]);

      // return waiter attention/unlock
      pthread_mutex_unlock(&arbiter_waiter);
   }

   // End of while/thread
   printf("PHILOSOPHER %d GOT TO EAT %d TIMES\n", philosopher_number, how_many_times_did_I_eat);
   return(NULL);
}

int main()
  {  int i;

   // Initialize the MEALS_ON_TABLE variable locking mutex
   pthread_mutex_init(&MEAL_COUNT_LOCK, NULL);

   // Initialize the arbiter_waiter variable locking mutex
   pthread_mutex_init(&arbiter_waiter, NULL);

   // Initialize all the chopsticks as "unlocked" or "not held"
   for(i=0;i<PHILOSOPHER_COUNT;i++) {
      pthread_mutex_init(&chopstick[i],NULL);
   }

   // Create the philosopher threads
   for(i=0;i<PHILOSOPHER_COUNT;i++) {
      // create pointers to numbers to be able to pass to thread
      int *arg = malloc(sizeof(*arg));
      if (arg == NULL){
         printf("Couldn't allocate memory for thread arg.\n");
         exit(EXIT_FAILURE);
      }
      *arg = i;
      pthread_create(&philosopher[i],NULL,(void *)philosopher_program,arg);
   }

   // join the philosophers when they are done
   for(i=0;i<PHILOSOPHER_COUNT;i++) {
      pthread_join(philosopher[i],NULL);
   }

   // Once all the threads terminate, destroy all the chopsticks
   for(i=0;i<PHILOSOPHER_COUNT;i++) {
      pthread_mutex_destroy(&chopstick[i]);
   }

   return 0;
   }

