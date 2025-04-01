#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"

// Create a new node
hashRecord* create_node(uint32_t hash, const char* name, uint32_t salary) {
    hashRecord* node = (hashRecord*)malloc(sizeof(hashRecord));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->hash = hash;
    strncpy(node->name, name, 49);
    node->name[49] = '\0';
    node->salary = salary;
    node->next = NULL;
    return node;
}

// Insert or update node in sorted order by hash
hashRecord* list_insert(hashRecord* head, uint32_t hash, const char* name, uint32_t salary) {
    hashRecord* curr = head;
    hashRecord* prev = NULL;

    while (curr && curr->hash < hash) {
        prev = curr;
        curr = curr->next;
    }

    if (curr && curr->hash == hash) {
        // Update existing node
        strncpy(curr->name, name, 49);
        curr->name[49] = '\0';
        curr->salary = salary;
        return head;
    }

    hashRecord* new_node = create_node(hash, name, salary);

    if (!prev) {
        new_node->next = head;
        return new_node;
    }

    prev->next = new_node;
    new_node->next = curr;
    return head;
}

// Delete node by hash
hashRecord* list_delete(hashRecord* head, uint32_t hash) {
    hashRecord* curr = head;
    hashRecord* prev = NULL;

    while (curr) {
        if (curr->hash == hash) {
            if (!prev) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return head;
        }
        prev = curr;
        curr = curr->next;
    }

    return head;
}

// Search for node by hash
hashRecord* list_search(hashRecord* head, uint32_t hash) {
    hashRecord* curr = head;
    while (curr) {
        if (curr->hash == hash) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Print the entire list
void list_print(FILE* file, hashRecord* head) {
    hashRecord* curr = head;
    while (curr) {
        fprintf(file, "%u,%s,%u\n", curr->hash, curr->name, curr->salary);
        curr = curr->next;
    }
}
