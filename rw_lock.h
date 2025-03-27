#include <semaphore.h>

typedef struct rwlock_t {
    sem_t writelock;
    sem_t lock;
    int num_readers;
} rwlock_t;

//set up reader-writer lock
void rwlock_init(rwlock_t *lock);

//grab the readlock
void rwlock_acquire_readlock(rwlock_t *lock);

//release the readlock
void rwlock_release_readlock(rwlock_t *lock);

//grab the writelock
void rwlock_acquire_writelock(rwlock_t *lock);

//release the writelock
void rwlock_release_writelock(rwlock_t *lock);
