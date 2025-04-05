#include <stdint.h>

//hashRecord has a hash value, a key (name), and a value (salary). Points to the next value
typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

void init_hashtable();

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

int hash_insert(int key, hashRecord value);

int hash_delete(int key);

hashRecord *hash_search(int key);
