#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"

#define HASH_TABLE_SIZE 5000

rwlock_t table_lock;  // Global read-write lock for synchronization
hashRecord *hash_table[HASH_TABLE_SIZE];

void init_hashtable(){
  for(int i=0; i<HASH_TABLE_SIZE; i++){
    hash_table[i] = NULL; // initalize 
  }
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

int hash_insert(int key, hashRecord value) {
  uint32_t hash = jenkins_one_at_a_time_hash((uint8_t*)value.name, strlen(value.name)) % HASH_TABLE_SIZE;
  rwlock_acquire_writelock(&table_lock);
  
  hashRecord **ptr = &hash_table[hash];
  while (*ptr) {
    if ((*ptr)->hash == hash) {
      (*ptr)->salary = value.salary;
      rwlock_release_writelock(&table_lock);
      return 0;  // Updated existing entry
    }
    ptr = &((*ptr)->next);
  }
  
  hashRecord *new_record = (hashRecord *)malloc(sizeof(hashRecord));
  if (!new_record) {
    rwlock_release_writelock(&table_lock);
    return -1;  // Memory allocation failure
  }
  
  *new_record = value;
  new_record->hash = hash;
  new_record->next = hash_table[hash];
  hash_table[hash] = new_record;
  
  rwlock_release_writelock(&table_lock);
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
