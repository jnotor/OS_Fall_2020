/** C file for RW Lock Library. writer-preferred version
 */
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "RW_lock.h"

/** Method to initialize the RW_lock_t struct and ready it for use. Sets
 * counters to 0, writer flag to false and initializes the cond and mutex pair
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_lock_init(RW_lock_t *lock){
    // Initialize num readers and writers to 0
    int num_readers_active = 0, num_writers_waiting = 0;

    // Initialize writer not active
    bool writer_active = false;

    // Initialize pthread vars
    pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
}


/** Method to lock before entering READ critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_read_lock(RW_lock_t *lock){
    // Lock global
    pthread_mutex_lock(&(lock->global_lock));

    // Chec for writers waiting or actively writing, spin if so
    while (lock->num_writers_waiting > 0 || lock->writer_active) {
        // cond_wait the cond/mutex pair
        pthread_cond_wait(&(lock->condition), &(lock->global_lock));
    }

    // Increment reader number
    lock->num_readers_active++;
}


/** Method to unlock at end of READ critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_read_unlock(RW_lock_t *lock){
    // Decrement readers
    lock->num_readers_active--;

    // Check for readers; if none, signal conditional var
    if (lock->num_readers_active == 0) {
        // Signal condition to release/wake those cond_waiting on it
        pthread_cond_signal(&(lock->condition));
    }

    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));

}


/** Method to lock before entering Write critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_write_lock(RW_lock_t *lock){
    // Lock global
    pthread_mutex_lock(&(lock->global_lock));

    // Increment num of writers
    lock->num_writers_waiting++;

    // Check for readers active or another writer active
    while (lock->num_readers_active > 0 || lock->writer_active == true) {
        // cond wait the condition and mutex pair
        pthread_cond_wait(&(lock->condition), &(lock->global_lock));
    }

    // Decrement num of writers
    lock->num_writers_waiting--;

    // Set writer_active to true
    lock->writer_active = true;
}


/** Method to unlock after exiting Write critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_write_unlock(RW_lock_t *lock) {
    // Set writer_active to false
    lock->writer_active = false;

    // Signal condition to release/wake those cond_waiting on it
    pthread_cond_signal(&(lock->condition));

    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));
}


