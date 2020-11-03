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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

/** Method to create philosopher eat/think process where a given philosopher, who
 * knows his own number, grabs the chopsticks to her/his right and left.  The
 * chopstick to the left of philosopher N is chopstick N.  The chopstick to the
 * right of philosopher N is chopstick N+1
 */
void *philosopher_program(int philosopher_number) {
   int how_many_times_did_I_eat = 0;

   while (MEALS_ON_TABLE > 0) {

      printf ("Philosopher %d is thinking\n", philosopher_number);
      usleep(1);

      printf ("Philosopher %d wants chopsticks\n",philosopher_number);

      // grab/lock chopsticks
      pthread_mutex_lock(&chopstick[philosopher_number]);
      pthread_mutex_lock(&chopstick[(philosopher_number + 1) % PHILOSOPHER_COUNT]);

      // eating
      printf ("Philosopher %d is ready to eat\n",philosopher_number);

      usleep(2);

      // done eating. put chopsticks back
      if (MEALS_ON_TABLE > 0) {
         // Increment thread access count
         how_many_times_did_I_eat++;

         // lock meal count mutex
         pthread_mutex_lock(&MEAL_COUNT_LOCK);
         // Decrement number of meals available
         MEALS_ON_TABLE--;
         printf ("Philosopher %d finished eating, %d meals remaining\n",philosopher_number, MEALS_ON_TABLE);

         // unlock meal count mutex
         pthread_mutex_unlock(&MEAL_COUNT_LOCK);
      }
      else {
         printf("Philosopher %d didn't eat because no food was left\n", philosopher_number);
      }

      // put down/unlock chopsticks
      pthread_mutex_unlock(&chopstick[philosopher_number]);
      pthread_mutex_unlock(&chopstick[(philosopher_number+1)%PHILOSOPHER_COUNT]);
      printf("Philosopher %d has placed chopsticks on the table\n", philosopher_number);
   }

   // End of while/thread
   printf("PHILOSOPHER %d GOT TO EAT %d TIMES\n", philosopher_number, how_many_times_did_I_eat);
   return(NULL);
}

int main()
  {  int i;

   // Initialize the MEALS_ON_TABLE variable locking mutex
   pthread_mutex_init(&MEAL_COUNT_LOCK, NULL);

   // Initialize all the chopsticks as "unlocked" or "not held"
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_mutex_init(&chopstick[i],NULL);

   // Create the philosopher threads
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_create(&philosopher[i],NULL,(void *)philosopher_program,(void *)i);

   // join the philosophers when they are done....  if the philosopher threads
   // never terminate, then these joins never happen.
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_join(philosopher[i],NULL);

   // Once all the threads terminate, destroy all the chopsticks
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_mutex_destroy(&chopstick[i]);

   return 0;
   }

