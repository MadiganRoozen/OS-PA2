
typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

int hash_insert(int key, hashRecord value);

int hash_delete(int key);

hashRecord hash_search(int key);
