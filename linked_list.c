#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "thread_manager.h"
#include "hash_table.h"
#include "linked_list.h"
#include "rw_lock.h"
#include "output.h"


// Create a new node, returns the node
node* create_node(hashRecord* rec) {
    node* new_node = (node*)malloc(sizeof(node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    new_node->record = rec;
    new_node->next = NULL;
    return new_node;
}

// Insert or update node in sorted order by hash, returns the head of the list
node* list_insert(node* new) {
    node* curr = list.head;
    //make the new node the head if there is not a head already
    if(curr == NULL){
        list.head = curr;
        return list.head;
    }

    uint32_t hash = new->record->hash;

    while (curr && curr->record->hash < hash) {
        curr = curr->next;
    }
    //if the current node hash is equal to the hash we are looking for, a node needs to be updated
    if (curr && curr->record->hash == hash) {
        //while the node we are on has a different key than the new node key
        while(curr->record->name != new->record->name){
            //move to the next entry in the hash_table record
            curr->record = curr->record->next;
        }
        // Update existing node
        curr->record = new->record;
        //get rid of new node
        free(new);
        return list.head;
    }
    else if(curr && curr->record->hash > hash){
        //add in new node
        node* old_node = curr->next;
        curr->next = new;
        new->next = old_node;
        return list.head;
    }
    return list.head;
    
}

// Delete node by hash, returns the head of the list
node* list_delete(node* to_delete) {
    node* curr = list.head;
    if(curr == to_delete){
        curr->next = list.head;
        free(curr);
    }
    while (curr) {
        if (curr->next == to_delete) {
            curr->next = to_delete->next;
            free(to_delete);
        }
    }
    return list.head;
}

// Search for node by hash, returns the node in question or NULL
node* list_search(uint32_t hash) {
    node* curr = list.head;
    while (curr) {
        if (curr->record->hash == hash) {
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