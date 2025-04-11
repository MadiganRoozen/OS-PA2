#include <stdio.h>
#include <string.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "rw_lock.h"
#include "output.h"

extern rwlock_t table_lock;

int main(int argc, char *argv[]){

  char* filename = argv[1];
  if(argc < 2){
    printf("Usage: must include executable and input file");
    return 0;
  }
  // just in case file is not there
  if(filename == NULL){
    printf("Must include 'commands.txt'");
    return 0;
  }
  // read in commands 
  FILE *input = fopen(filename, "r");
  if (input == NULL) {
    printf("Error opening file");
    return 0;
  }

  // read in number of threads -- first line 
  char first[300];
  int thread_count = 0;
  if (fgets(first, sizeof(first), input)) {
    sscanf(first, "threads,%d,0", &thread_count);
  }

  // read how many commands for initalizing 
  int command_lines = 0;
  while (fgets(first, sizeof(first), input)) {
    command_lines++;
  }

  // rewind so we can go back and enqueue them later 
  rewind(input);
  fgets(first, sizeof(first), input);

  // initalize hash linked list and thread manager 
  output_init();
  init_hashtable();
  init_thread_manager(command_lines);

  //reader-writer lock for the function to use while executing commands
  //all threads should share this one lock. When they try to do an insert or a delete they should try to acquire the writelock. When they do a search they should acquire the readlock
  // rwlock_t* lock;
  rwlock_init(&table_lock);


  // get rest of commands 
  while (fgets(first, sizeof(first), input)) {
    if (strlen(first) > 1) {
      char *cmd = strdup(first);
      enqueue_command(cmd);
    }
  }

  // start threads -- thread_manager 
  start_threads();

  // print output 
  output_final();
  
  fclose(input); // close file
  return 0;
}
