/** C file for RW Lock Library. Reader-preferred version
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "RW_lock_reader_pref.h"

/** Method to initialize the RW_lock_t struct and ready it for use. Sets
 * read_counter to 0, and initializes both mutexes, read_protector and global_lock
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_lock_init(RW_lock_t *lock){
    pthread_mutex_t read_protector = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
    int reader_counter = 0;
}


/** Method to lock before entering READ critical section. Locks read_protector
 * before incrementing the read_counter and performing logic to see if the
 * global_lock should be locked, then unlocks read_protector after doing so or not
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_read_lock(RW_lock_t *lock){
    // Lock reader protector
    pthread_mutex_lock(&(lock->read_protector));

    // Increment reader counter
    lock->reader_counter++;

    // If there's a reader (or more), lock the global lock so no writes occur
    if (lock->reader_counter == 1){
        pthread_mutex_lock(&(lock->global_lock));
    }

    // Unlock the reader_protector, as we've completed inc. counter &
    // potentially locking global
    // pthread_mutex_unlock(&(lock->read_protector));
}


/** Method to unlock at end of READ critical section. Locks read_protector before
 * decrementing the read_counter and performing logic to see if the global_lock
 * can be unlocked, then unlocks read_protector after doing so or not
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_read_unlock(RW_lock_t *lock){
    // Lock reader protector
    // pthread_mutex_lock(&(lock->read_protector));

    // Decrement reader counter
    lock->reader_counter--;

    // If no readers, unlock global so writer can write, potentially
    if (lock->reader_counter == 0){
        pthread_mutex_unlock(&(lock->global_lock));
    }

    // Unlock the reader_protector, as we've completed dec. counter &
    // potentially unlocking global
    pthread_mutex_unlock(&(lock->read_protector));
}


/** Method to lock before entering Write critical section. Simply locks struct's
 * global_lock member
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_write_lock(RW_lock_t *lock){
    pthread_mutex_lock(&(lock->global_lock));
}


/** Method to unlock after exiting Write critical section. Simply unlocks struct's
 * global_lock member
 *
 * :param lock: pointer to RW_lock_t
 * :param type: struct RW_lock_s
 */
void RW_write_unlock(RW_lock_t *lock) {
    pthread_mutex_unlock(&(lock->global_lock));
}


