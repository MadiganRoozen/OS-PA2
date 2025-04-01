#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"

// Help Perform the required hash table operations ? I didnt see it anywhere in our files
extern void execute_commands(char *command);

// Global variables for managing the command queue.
static char **command_array = NULL;  // Array of command strings.
static int command_count = 0;        // How many commands have been enqueued.
static int total_commands = 0;       // Total number of commands, already shown in the first line 

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

// Enqueue a command string into the global command array.
void enqueue_command(char *command) {
    if (command_count < total_commands) {
        command_array[command_count++] = command;
    } else {
        fprintf(stderr, "Error: Exceeded allocated command array size.\n");
        exit(EXIT_FAILURE);
    }
}

// Thread function: each thread processes one command.
void *process_command(void *arg) {
    char *cmd = (char *)arg;
    execute_commands(cmd); // call the execute_commands to allow the thread manager code to pass the work of interpreting each command. 
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
