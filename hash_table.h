#include <stdint.h>

//hashRecord has a hash value, a key (name), and a value (salary). Points to the next value
typedef struct hash_struct
{
  uint32_t hash;
  char name[50]; //key
  uint32_t salary; //value
  struct hash_struct *next;
} hashRecord;

typedef struct list{
  hashRecord *head;
} hash_table;

extern hash_table list;

void init_hashtable();

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

int hash_insert(char* key, uint32_t value);

int hash_delete(char* key);

hashRecord *hash_search(char* key);
