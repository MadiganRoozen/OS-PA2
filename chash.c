#include <stdio.h>
#include <thread_manager.h>
#include <hash_table.h>
#include <linked_list.h>
#include <rw_lock.h>
#include <output.h>

int main(){
  //reader-writer lock for the function to use while executing commands
  //all threads should share this one lock. When they try to do an insert or a delete they should try to acquire the writelock. When they do a search they should acquire the readlock
  rwlock_t* lock;
  rwlock_init(lock);

  return 0;
}
