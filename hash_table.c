#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "rw_lock.h"
#include "output.h"

hash_table list;
rwlock_t table_lock;  // Global read-write lock for synchronization

void init_hashtable(){
  list.head = NULL;
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

//not messing with the list, does not need to lock directly
hashRecord* create_record(uint32_t hash, char* key, uint32_t value){
  hashRecord *new_record = (hashRecord *)malloc(sizeof(hashRecord));

  if (!new_record) {
    printf("memeory allocation for new node failed\n");
    return NULL;  // Memory allocation failure
  }
  new_record->hash = hash;
  strncpy(new_record->name, key, sizeof(new_record->name) - 1);
  new_record->name[sizeof(new_record->name) - 1] = '\0';
  new_record->salary= value;
  new_record->next = NULL; 
  return new_record;
}

//Im not sure the locks need to go in here, i would put them in the thread manager
int hash_insert(char* key, uint32_t value) {
  //calc has value using key (which is the string name)
  uint32_t hash = jenkins_one_at_a_time_hash((uint8_t*)key, strlen(key));

  //try to find the record
  //hashRecord *existing_record = hash_search(key);

  rwlock_acquire_writelock(&table_lock);
  hashRecord *cur = list.head;
  printf("list head %p\n", (void*)cur);

  if(cur == NULL){
    printf("HEAD IS NULL\n");
    //make new node and make it the head
    list.head = create_record(hash, key, value);
    printf("inserted as head\n");
    rwlock_release_writelock(&table_lock);
    return 0;
  }

  hashRecord* existing_record = NULL;
  while (cur != NULL) {
    if (cur->hash == hash) {
      existing_record = cur;
      break; //found
    }
    if (cur->hash > hash) {
      existing_record = NULL;
      break;
    }
    cur = cur->next;
  }
  printf("exisitng record is %p\n", (void*)existing_record);

  if(existing_record == NULL){
    //make new node and add it to the list
    hashRecord* new = create_record(hash, key, value);

    if(list.head != NULL && list.head->hash > new->hash){
      hashRecord *old_head = list.head;
      list.head = new;
      new->next = old_head;
      printf("inserted as head\n");
      rwlock_release_writelock(&table_lock);
      return 0;
    }
    
    cur = list.head;
    while(cur->next != NULL && cur->next->hash < new->hash){
      cur = cur->next;
    }
    hashRecord* old_next = cur->next;
    cur->next = new;
    new->next = old_next;
    printf("inserted\n");
    rwlock_release_writelock(&table_lock);
    return 0;
  }
  else{
    //update the found record, only need to update the value
    existing_record->salary = value;
    printf("updated\n");
    rwlock_release_writelock(&table_lock);
    return 0;
  }

  printf("not inserted\n");
  rwlock_release_writelock(&table_lock);
  return -1;  // Inserted new entry
}

int hash_delete(char* key) {
  uint32_t hash = jenkins_one_at_a_time_hash((uint8_t*)key, strlen(key));
  rwlock_acquire_writelock(&table_lock);

  hashRecord* cur = list.head;
  if(cur == NULL){
    printf("nothing deleted, head was null\n");
    rwlock_release_writelock(&table_lock);
    return -1;
  }
  //delete the head
  if(list.head->hash == hash){
    hashRecord* old_head = list.head;
    list.head = list.head->next;
    free(old_head);
    rwlock_release_writelock(&table_lock);
    return 0;
  }
  while(cur != NULL && cur->next != NULL && cur->next->hash < hash){
    cur = cur->next;
  }
  if(cur == NULL || cur->next == NULL || cur->next->hash != hash){
    rwlock_release_writelock(&table_lock);
    return -1; //nothing deleted
  }
  hashRecord * new_next = cur->next->next;
  free(cur->next);
  cur->next = new_next;
  
  rwlock_release_writelock(&table_lock);
  return 0;  
}

hashRecord* hash_search(char* key) {
  printf("searching for %s\n", key);
  uint32_t hash = jenkins_one_at_a_time_hash((uint8_t*)key, strlen(key));
  rwlock_acquire_readlock(&table_lock);
  
  hashRecord* cur = list.head;
  while (cur != NULL) {
    if (cur->hash == hash) {
      printf("found\n");
      rwlock_release_readlock(&table_lock);
      return cur; //found
    }
    if (cur->hash > hash) {
      break;
    }
    cur = cur->next;
  }

  printf("not found\n");
  rwlock_release_readlock(&table_lock);
  return NULL; //not found
}

