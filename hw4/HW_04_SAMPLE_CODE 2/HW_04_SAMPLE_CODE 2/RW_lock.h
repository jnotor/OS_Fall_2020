/** Header file for RW Lock Library. writer-preferred version
 */
#include <pthread.h>
#include <stdbool.h>


/** Structure to contain a pthread mutex and cond pair, 2 int counters for readers
 * and writers, respectively, and a boolean flag for whether or not a writer is
 * active
 */
typedef struct RW_lock_s {
    int num_readers_active;
    int num_writers_waiting;

    // Whether a writer has acquired the lock
    bool writer_active;

    // Globally locking mutex
    pthread_mutex_t global_lock;

    // condition to wait
    pthread_cond_t condition;

} RW_lock_t;


void RW_lock_init(RW_lock_t *lock);

void RW_read_lock(RW_lock_t *lock);

void RW_read_unlock(RW_lock_t *lock);

void RW_write_lock(RW_lock_t *lock);

void RW_write_unlock(RW_lock_t *lock);
