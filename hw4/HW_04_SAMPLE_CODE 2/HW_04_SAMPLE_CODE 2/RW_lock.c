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

    // if there are writers waiting or actively writing, wait on conditional
    while (lock->num_writers_waiting > 0 || lock->writer_active){
        pthread_cond_wait(&(lock->condition), &(lock->global_lock));
    }

    // Increment reader number
    lock->num_readers_active++;
    printf("readers: %d\n", lock->num_readers_active);

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

    // Decrement readers
    lock->num_readers_active--;
    // printf("NUM Readers: %d\n", lock->num_readers_active);

    // Check for readers; if none, signal conditional var
    if (lock->num_readers_active == 0) {
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
    printf("NUM writers: %d\n", lock->num_writers_waiting);
    printf("NUM readers: %d\n", lock->num_readers_active);
    printf("writer active: %s\n", lock->writer_active ? "true" : "false");

    // Wait on cond if there are readers active or another writer active
    while (lock->num_readers_active > 0 || lock->writer_active == true){
        printf("here\n");
        pthread_cond_wait(&(lock->condition), &(lock->global_lock));
    }

    // Decrement num of writers
    lock->num_writers_waiting--;

    // Set writer_active to true
    lock->writer_active = true;
    printf("Num writers: %d\n", lock->num_writers_waiting);

    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));
    printf("num writers: %d\n", lock->num_writers_waiting);
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

    // Signal condition
    int ret;
    ret = pthread_cond_signal(&(lock->condition));
    printf("%d\n", ret);

    // Unlock global
    pthread_mutex_unlock(&(lock->global_lock));
}


