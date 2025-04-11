#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include "sys/time.h"
#include <string.h>
#include <pthread.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "rw_lock.h"
#include "output.h"

FILE *output;
int lock_acquisitions = 0;
int lock_releases = 0;
static pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

// prepare output file to write in 
void output_init(int threads){
    output = fopen("output.txt", "w");
    
    // in case file won't open 
    if (output == NULL){
        printf("'output.txt' not found");
    }
    fprintf(output, "Running %d threads\n", threads);
    fflush(output);
}

// getting timestamp in microseconds
uint64_t get_time() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    uint64_t microseconds = ((uint64_t)te.tv_sec * 1000000) + te.tv_usec; // calculate milliseconds
    return microseconds;
  }

// writes out command as executed -- thread manager 
void output_write_command(char* cmd, char* name, int salary){
    pthread_mutex_lock(&output_lock);
    uint64_t time = get_time();
    fprintf(output, "%" PRIu64 ",%s,%s,%d\n", time, cmd, name, salary);
    fflush(output);
    pthread_mutex_unlock(&output_lock);
}

// write out when locks are acquired + released -- rw_lock
void output_lock_status(char* status){
    pthread_mutex_lock(&output_lock);
    uint64_t time = get_time();
    fprintf(output, "%" PRIu64 ",%s\n", time, status);
    fflush(output);
    pthread_mutex_unlock(&output_lock);

    // count the lock statuses for later 
    if (strstr(status, "ACQUIRED")){
        lock_acquisitions++;
    }
    if (strstr(status, "RELEASED")){
        lock_releases++;
    }
}

// write out when an operation is waiting on a CV  + when it's signaling 
void output_condition_variables(char* signal){
    pthread_mutex_lock(&output_lock);
    uint64_t time = get_time();
    fprintf(output, "%" PRIu64 ",%s\n", time, signal);
    fflush(output);
    pthread_mutex_unlock(&output_lock);
}

// prints just table 
void print_table(){
    rwlock_acquire_readlock(&table_lock);
    pthread_mutex_lock(&output_lock);
    // list of records sorted by hash values 
    hashRecord* curr = list.head; 
    while (curr) {
        fprintf(output, "%u,%s,%d\n", curr->hash, curr->name, curr->salary);
        curr = curr->next;
    } 

    pthread_mutex_unlock(&output_lock);
    rwlock_release_readlock(&table_lock);
}

// final print
void output_final(){
    fprintf(output, "\nNumber of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(output, "Number of lock releases: %d\n", lock_releases);

    // list of records sorted by hash values 
    print_table();

    fclose(output);
}