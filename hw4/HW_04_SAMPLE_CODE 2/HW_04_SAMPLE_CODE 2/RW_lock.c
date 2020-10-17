/** C file for RW Lock Library. writer-preferred version
 */
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "RW_lock.h"

/** Method to initialize the RW_lock_t struct and ready it for use. Sets
 * read_counter to 0, and initializes both mutexes, read_protector and global_lock
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_lock_init(RW_lock_t *lock){
    int num_readers_active = 0, num_writers_waiting = 0;
    bool writer_active = false;
    pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
}


/** Method to lock before entering READ critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_read_lock(RW_lock_t *lock){
    // Lock global
    pthread_mutex_lock(&(lock->global_lock));

    while (lock->num_writers_waiting > 0 || lock->writer_active){
        // wait(cond, lock->global_lock)
    }

    lock->num_readers_active++;

    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));
}


/** Method to unlock at end of READ critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_read_unlock(RW_lock_t *lock){
    // Lock global
    pthread_mutex_lock(&(lock->global_lock));
    lock->num_readers_active--;

    if (lock->num_readers_active == 0){
        // Notify cond (broadcast). TODO: what? in think its his main, c?
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
    while (lock->num_readers_active > 0 || lock->writer_active){
        // wait(cond, lock->global_lock) // TODO
    }

    // Decrement num of writers
    lock->num_writers_waiting--;

    // Set writer_active to true
    lock->writer_active = true;

    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));
}


/** Method to unlock after exiting Write critical section.
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_write_unlock(RW_lock_t *lock) {
    // Lock global
    pthread_mutex_lock(&(lock->global_lock));

    // Set writer_active to false
    lock->writer_active = false;

    // Notify cond (broadcast). TODO: what? in think its his main, c?


    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));
}


