#include <stdint.h>
#include <stdio.h>

hashRecord* list_insert(hashRecord* head, uint32_t hash, const char* name, uint32_t salary);
hashRecord* list_delete(hashRecord* head, uint32_t hash);
hashRecord* list_search(hashRecord* head, uint32_t hash);
void list_print(FILE* file, hashRecord* head);

