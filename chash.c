#include <stdio.h>
#include <string.h>

#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"

int main(){
  // read in commands 
  FILE *input = fopen("commands.txt", "r");

  // just in case file is not there
  if(input == NULL){
    printf("'commands.txt' not Found");
  }

  // read in number of threads -- first line 
  char first[20];
  int thread_count = 0;
  if (fgets(first, sizeof(first), input)) {
    sscanf(first, "threads,%d,0", &thread_count);
  }

  // initalize hash linked list and thread manager 

  //reader-writer lock for the function to use while executing commands
  //all threads should share this one lock. When they try to do an insert or a delete they should try to acquire the writelock. When they do a search they should acquire the readlock
  rwlock_t* lock;
  rwlock_init(lock);

  // get rest of commands 
  while (fgets(first, sizeof(first), input)) {
    char *cmd = strdup(first);
    enqueue_command(cmd); // would be in thread_manager
  }

  // start threads -- thread_manager 

  // print output 
  output_final();

  fclose(input); // close file
  return 0;
}
