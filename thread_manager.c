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
        hash_insert(name, (uint32_t)salary);
        output_write_command("INSERT", name, salary);
    } 
    else if (strcmp(command, "delete") == 0) {
        // hash value for the key and call delete.
        //uint32_t key_hash = jenkins_one_at_a_time_hash((uint8_t*)name, strlen(name));
        //hash_delete(key_hash);
        hash_delete(name);
        output_write_command("DELETE", name, salary);
    } 
    else if (strcmp(command, "search") == 0) {
        //  hash value for the key and call search.
        //uint32_t key_hash = jenkins_one_at_a_time_hash((uint8_t*)name, strlen(name));
        //hashRecord *result = hash_search(key_hash);
        hash_search(name);
        output_write_command("SEARCH", name, salary);
       
    } 
    else if (strcmp(command, "print") == 0) {
        // Print the entire table.
        print_table();
        output_write_command("PRINT", "", 0);
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
