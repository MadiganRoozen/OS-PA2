#include <stdint.h>
#include <stdio.h>

typedef struct node {
    hashRecord *record;   
    struct node *next;   
} node;

typedef struct list{
    node *head;
} linked_list;

node* create_node(hashRecord* rec);
node* list_insert(node* new);
node* list_delete(node* to_delete);
node* list_search(uint32_t hash);
void list_print(FILE* file, hashRecord* head);
node* list_sort();

extern linked_list list;
