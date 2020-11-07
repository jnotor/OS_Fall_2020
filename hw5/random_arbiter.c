/** This is an example of standard dining philosophers. This program implements
 * a modified dining philosophers problem in which each philosopher can choose
 * to use ANY two chopsticks (forks) on the table. As before, each philosopher
 * is implemented as a separate pthread. This version also has a “finite number
 * of meals”. All threads will end when that number of meals have been consumed.
 * In this version, there are 10 philosophers and 10 million meals available.
 * As there is no deadlock protection. This code will DEFINITELY deadlock before
 * all 10 million meals are consumed.
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
 * knows his own number, decides he wants two chopsticks at random.  He will at
 * least choose two DIFFERENT chopsticks
 *
 * @param philosopher_number_ptr: ptr to the number of the philosopher
 */
void *philosopher_program(void *philosopher_number_ptr) {
   int philosopher_number = *((int *) philosopher_number_ptr);
   free(philosopher_number_ptr);

   int how_many_times_did_I_eat = 0;

   int chopstick_1;
   int chopstick_2;

   while (MEALS_ON_TABLE > 0) {
      usleep(1);

      chopstick_1 = rand() % PHILOSOPHER_COUNT;

      // Ensure philosopher grabs 2 different chopsticks
      do {
         chopstick_2 = rand() % PHILOSOPHER_COUNT;
      } while (chopstick_1 == chopstick_2);


      // get waiter attention/lock
      pthread_mutex_lock(&arbiter_waiter);

      // grab/lock chopsticks
      pthread_mutex_lock(&chopstick[chopstick_1]);
      pthread_mutex_lock(&chopstick[chopstick_2]);

      // eating
      usleep(2);

      // done eating. put chopsticks
      if (MEALS_ON_TABLE > 0) {
         // Increase count of how many times thread "ate"
         how_many_times_did_I_eat++;

         // Lock meal count mutex
         pthread_mutex_lock(&MEAL_COUNT_LOCK);

         // Decrement meals left


         // Unlock meal count mutex
         pthread_mutex_unlock(&MEAL_COUNT_LOCK);
      }

      // unlock/put down chopsticks
      pthread_mutex_unlock(&chopstick[chopstick_1]);
      pthread_mutex_unlock(&chopstick[chopstick_2]);
      printf("Philosopher %d has placed chopsticks on the table\n", philosopher_number);

      // return waiter attention/unlock
      pthread_mutex_unlock(&arbiter_waiter);
   }

   // End of while/thread
   printf("PHILOSOPHER %d GOT TO EAT %d TIMES\n", philosopher_number, how_many_times_did_I_eat);
   return(NULL);
}

int main()
  {  int i;

   // Seed the random number generator
   srand(time(NULL));

   // Initialize the MEALS_ON_TABLE variable locking mutex
   pthread_mutex_init(&MEAL_COUNT_LOCK, NULL);

   // Initialize the arbiter_waiter variable locking mutex
   pthread_mutex_init(&arbiter_waiter, NULL);

   // Initialize all the chopsticks as "unlocked" or "not held"
   for(i=0;i<PHILOSOPHER_COUNT;i++) {
      pthread_mutex_init(&chopstick[i],NULL);
   }

   // Create the philosopher threads
   for(i=0;i<PHILOSOPHER_COUNT;i++){
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

