#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "thread_worker_types.h"

// contains definitions for data structures needed for implementing linked lists

struct Node {
    struct TCB* data;
    struct Node* next;
};

struct LinkedList {
    struct Node* head;
    int count;
};

// initializes linked list object
void* createList();

// prints out queue
void* printList();

// creates a new node and returns it; parameter is a TCB struct
void* addToQueue(struct TCB* thread);

#endif
