#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"

hash_table list;
rwlock_t table_lock;  // Global read-write lock for synchronization

void init_hashtable(){
  list.head = NULL;
}

hashRecord* create_record(uint32_t hash, char* key, uint32_t value){
  hashRecord *new_record = (hashRecord *)malloc(sizeof(hashRecord));
  if (!new_record) {
    rwlock_release_writelock(&table_lock);
    return -1;  // Memory allocation failure
  }
  new_record->hash = hash;
  strncpy(new_record->name, key, sizeof(new_record->name) - 1);
  new_record->name[sizeof(new_record->name) - 1] = '\0';
  new_record->salary= value;
  new_record->next = NULL; 
  return new_record;
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

//Im not sure the locks need to go in here, i would put them in the thread manager
int hash_insert(char* key, uint32_t value) {
  //calc has value using key (which is the string name)
  uint32_t hash = jenkins_one_at_a_time_hash((uint8_t*)key, strlen(key));
  hashRecord *cur = list.head;

  if(cur == NULL){
    //make new node and make it the head
  }

  hashRecord *existing_record = hash_search(key);

  if(existing_record == NULL){
    //make new node and add it to the list
  }
  else{
    //update the found record
  }

  /*
  hashRecord **ptr = &hash_table[hash];
  while (*ptr) {
    if ((*ptr)->hash == hash) {
      (*ptr)->salary = value.salary;
      rwlock_release_writelock(&table_lock);
      return 0;  // Updated existing entry
    }
    ptr = &((*ptr)->next);
  }
  */
  return 0;  // Inserted new entry
}

int hash_delete(int key) {
  uint32_t hash = key % HASH_TABLE_SIZE;
  rwlock_acquire_writelock(&table_lock);
  
  hashRecord **ptr = &hash_table[hash];
  while (*ptr) {
    if ((*ptr)->hash == hash) {
      hashRecord *temp = *ptr;
      *ptr = (*ptr)->next;
      free(temp);
      rwlock_release_writelock(&table_lock);
      return 0;  // Successfully deleted
    }
    ptr = &((*ptr)->next);
  }
  
  rwlock_release_writelock(&table_lock);
  return -1;  // Key not found
}

hashRecord *hash_search(int key) {
  uint32_t hash = key % HASH_TABLE_SIZE;
  rwlock_acquire_readlock(&table_lock);
  
  hashRecord *ptr = hash_table[hash];
  while (ptr) {
    if (ptr->hash == hash) {
      rwlock_release_readlock(&table_lock);
      return ptr;  // Found entry
    }
    ptr = ptr->next;
  }
  
  rwlock_release_readlock(&table_lock);
  return NULL;  // Not found
}