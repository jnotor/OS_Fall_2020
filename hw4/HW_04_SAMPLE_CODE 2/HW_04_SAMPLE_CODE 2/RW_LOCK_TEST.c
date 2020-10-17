/** Code is intended to run N threads.  One of those threads, filler, fills a large
 * array with numbers (initially zeros). Filler iterates through array & fills
 * each slot of the array with a value equal to the index. The other N-1 threads
 * are "readers".  In this case, all of these threads are constantly computing a
 * "checksum"; see if that sum is consistent with what one would get evaluating a
 * summation formula that should give the same result.  If at any point, a "reader"
 * thread doesn't see what it expects, it complains IMMEDIATELY by printing an error
 * message to standard output.
 */

/** TODO: Maybe you might want to run this code with NO sync primitives, with standard mutex,
* and with your reader-writer locks.  It would be interesting to use the unix time
* command to see how long the code takes to complete under various situations and with
* various numbers of reader threads.  With this code, you'll find a simple shell
* script that you can use to run the program multiple times.  You could pipe the shell's
* output to a file to make records of the results of multiple experiments.  You could
* include time commands in my shell to capture records of run time....
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "RW_lock.c"

#define  ARRAY_SIZE  1000

// array to be filled and checked
int big_array[ARRAY_SIZE];

// Tracking var for num items "read" into above array;
// incremented by filler, monitored by reader for checksum use
int Data_Insert_Count = 0;

// Init lock
RW_lock_t array_lock;


/**
 * Method to associate with filler thread. Iterates until array full.
 */
void *put_item_in_array() {
   while (Data_Insert_Count < ARRAY_SIZE) {
      // Lock for writing
      RW_write_lock(&array_lock);

      // Set big_array at index Data_Insert_Count = Data_Insert_Count and then inc.
       big_array[Data_Insert_Count] = Data_Insert_Count;
       Data_Insert_Count++;

      // Unlock from writing
      RW_write_unlock(&array_lock);
   }
}


/** Method will CONSTANTLY run through the big_array, sum its contents, and
 * check if that sum is consistent with what SHOULD be there as predicted by
 * evaluating the proper summation formula. Runs until array full.
 *
 * Should not perform the above is any thread is writing; can do while others read
 */
void *array_checksum() {
   int index, sum, sum_predicted;

   while (Data_Insert_Count < ARRAY_SIZE) {
      // Lock for reading
      RW_read_lock(&array_lock);

      // Zero out sum per iteration
      sum = 0;

      // Calculate sum via Sigma iteration
      for (index=1; index < ARRAY_SIZE; index++){
         sum += big_array[index];
      }

      // Calculate sum using formula
      sum_predicted = ((Data_Insert_Count-1) * (Data_Insert_Count))/2;

      // Compare two sums
      if (sum != sum_predicted){
         printf("ERROR: INCONSISTENCY IN BIG_ARRAY!  sum=%d   predicted sum =%d  items_inserted=%d\n", sum, sum_predicted, Data_Insert_Count);
      }

      // Unlock from reading
      RW_read_unlock(&array_lock);
   }
}



int main() {
   // NOTE: more readers = more contention for use of single array resource
   // num reader threads declaration
   int check_thread_count = 10;
   // used for generic counts
   int c;

   // The descriptor for the single "writer" thread
   pthread_t put_thread;
   // Descriptors for up to 100 "reader" threads
   pthread_t check_thread[100];

   // Init the RW lock
   RW_lock_init(&array_lock);

   // Zero out the contents of the big_array
   for (c=0; c < ARRAY_SIZE; c++)
      big_array[c] = 0;

   // launch 1 filler thread
   pthread_create(&put_thread,   NULL, &put_item_in_array, NULL);

   // launch num of reader threads == check_thread_count
   for (c = 0; c < check_thread_count; c++){
      pthread_create(&check_thread[c], NULL, &array_checksum, NULL);
   }

   // join filler thread
   pthread_join(put_thread, NULL);

   // join all reader threads
   for (c = 0; c < check_thread_count; c++)
       pthread_join(check_thread[c], NULL);
}
