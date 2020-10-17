/** Header file for RW Lock Library. Reader-preferred version
 */
#include <pthread.h>

/** Structure to contain 2 mutexes and a single integer counter to perform read-
 * preferred RW locking
 */
typedef struct RW_lock_s {
    // Int to keep track of the number of readers. Accessed by read functs.
    int reader_counter;

    // Reader mutex lock use to protect reader_counter. Accessed by read functs.
    pthread_mutex_t read_protector;

    // Global mutex lock that provides mutual exlusion of writers. Accessed by
    // both read and write locking functions
    pthread_mutex_t global_lock;

} RW_lock_t;


void RW_lock_init(RW_lock_t *lock);

void RW_read_lock(RW_lock_t *lock);

void RW_read_unlock(RW_lock_t *lock);

void RW_write_lock(RW_lock_t *lock);

void RW_write_unlock(RW_lock_t *lock);
