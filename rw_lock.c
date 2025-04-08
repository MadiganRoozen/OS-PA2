#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "rw_lock.h"
#include "output.h"

//set up the lock with no readers and a semaphore for each lock and writelock
void rwlock_init(rwlock_t *lock) {
    lock->num_readers = 0;
    sem_init(&lock->lock, 0, 1); 
    sem_init(&lock->writelock, 0, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    sem_wait(&lock->lock);
    lock->num_readers++;
    
    if (lock->num_readers == 1)
	    sem_wait(&lock->writelock);

    sem_post(&lock->lock);
    output_lock_status("READ LOCK ACQUIRED");
}

void rwlock_release_readlock(rwlock_t *lock) {
    sem_wait(&lock->lock);
    lock->num_readers--;

    if (lock->num_readers == 0)
	    sem_post(&lock->writelock);

    sem_post(&lock->lock);
    output_lock_status("READ LOCK RELEASED");
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->writelock);
    output_lock_status("WRITE LOCK ACQUIRED");
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->writelock);
    output_lock_status("WRITE LOCK RELEASED");
}


