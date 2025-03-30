#include <stdio.h>
#include <time.h>
#include <string.h>

#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"

FILE *output;
int lock_acquisitions = 0;
int lock_releases = 0;

// prepare output file to write in 
void output_init(){
    output = fopen("output.txt", "w");
    
    // in case file won't open 
    if (output == NULL){
        printf("'output.txt' not found");
    }
}

// getting timestamp in milliseconds
long get_time(){
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

// writes out command as executed 
void output_write_command(char* cmd, char* name, int salary){

    int time = get_time();
    fprintf(output, "%ld,%s,%s,%d\n", time, cmd, name, salary);
    fflush(output);

}

// write out when locks are acquired + released
void output_lock_status(char* status){
    int time = get_time();
    fprintf(output, "%ld,%s\n", time, status);
    fflush(output);

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
    int time = get_time();
    fprintf(output, "%ld,%s\n", time, signal);
    fflush(output);
}

// final print
void output_final(){

    fprintf(output, "\nNumber of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(output, "Number of lock releases: %d\n", lock_releases);

    // list of records sorted by hash values 
    //hashRecord* records = sorted_table(); // from hash_table.h

    // while (records) {
    //     fprintf(output, "%ld,%s,%d", )
    // } 

}