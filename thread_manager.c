#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "rw_lock.h"
#include "output.h"

// Global variables for managing the command queue.
static char **command_array = NULL;  
static int command_count = 0;      
static int total_commands = 0;      

// Initialize the thread manager by allocating an array to hold the commands
void init_thread_manager(int total) {
    total_commands = total;
    command_array = (char **)malloc(total_commands * sizeof(char *));
    if (command_array == NULL) {
        fprintf(stderr, "Error: Memory allocation for command array failed.\n");
        exit(EXIT_FAILURE);
    }
    command_count = 0;
}



// parses the command string and calls the corresponding hash table function.
void execute_commands(char *cmd) {
    char command[20], name[50];
    int salary;
    
    // expected format "command,name,salary"
    sscanf(cmd, "%[^,],%[^,],%d", command, name, &salary);
    
    if (strcmp(command, "insert") == 0) {
        //  hash table insert function 
        printf("inserting %s\n", name);
        output_write_command("INSERT", name, salary);
        hash_insert(name, (uint32_t)salary);
    } 
    else if (strcmp(command, "delete") == 0) {
        // hash value for the key and call delete.
        //uint32_t key_hash = jenkins_one_at_a_time_hash((uint8_t*)name, strlen(name));
        //hash_delete(key_hash);
        printf("deleting %s\n", name);
        output_write_command("DELETE", name, salary);
        hash_delete(name);
    } 
    else if (strcmp(command, "search") == 0) {
        //  hash value for the key and call search.
        //uint32_t key_hash = jenkins_one_at_a_time_hash((uint8_t*)name, strlen(name));
        //hashRecord *result = hash_search(key_hash);
        printf("searching %s\n", name);
        output_write_command("SEARCH", name, salary);
        hash_search(name);
       
    } 
    else if (strcmp(command, "print") == 0) {
        // Print the entire table.
        output_write_command("PRINT", "", 0);
        print_table();
    }
}




// Enqueue a command string into the global command array.
void enqueue_command(char *command) {
    if (command_count < total_commands) {
        command_array[command_count++] = command;
    } else {
        fprintf(stderr, "Error: Exceeded allocated command array size.\n");
        exit(EXIT_FAILURE);
    }
}

void *process_command(void *arg) {
    char *cmd = (char *)arg;
    execute_commands(cmd); 
    return NULL;
}

/*
// Create a thread for each enqueued command, wait for them to finish, and then clean up.
void start_threads() {
    pthread_t *threads = malloc(command_count * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Error: Memory allocation for threads failed.\n");
        exit(EXIT_FAILURE);
    }
    
    // Create threads.
    for (int i = 0; i < command_count; i++) {
        int ret = pthread_create(&threads[i], NULL, process_command, (void *)command_array[i]);
        if (ret != 0) {
            fprintf(stderr, "Error: pthread_create failed for command %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    
    // Wait (join) for all threads to finish.
    for (int i = 0; i < command_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
    free(command_array);
    command_array = NULL;
}
*/

// Helper lambda to run threads for a given command type
void run_phase(const char *type, pthread_t* threads) {
    int thread_index = 0;
    for (int i = 0; i < command_count; i++) {
        if (strncmp(command_array[i], type, strlen(type)) == 0) {
            int ret = pthread_create(&threads[thread_index], NULL, process_command, (void *)command_array[i]);
            if (ret != 0) {
                fprintf(stderr, "Error: pthread_create failed for command %d\n", i);
                exit(EXIT_FAILURE);
            }
            thread_index++;
        }
    }
    // Join only the threads that were created for this phase
    for (int i = 0; i < thread_index; i++) {
        pthread_join(threads[i], NULL);
    }
}

void start_threads() {
    pthread_t *threads = malloc(command_count * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Error: Memory allocation for threads failed.\n");
        exit(EXIT_FAILURE);
    }

    // Run all inserts first
    run_phase("insert", threads);
    // Then deletes
    run_phase("delete",threads);
    // Then any other command (search, print, etc.)
    run_phase("search",threads);
    run_phase("print",threads);

    free(threads);
    free(command_array);
    command_array = NULL;
}
