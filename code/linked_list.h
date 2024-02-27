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

// prints out the number of items in the queue
void* printCount();

// initializes linked list object
void* createList();

// prints out queue
void* printList();

// creates a new node and returns it; parameter is a TCB struct
void* addToQueue(struct TCB* thread);

// moves first node to the end
int popAndPlop();

// returns the value of a new thread id
int newThreadId();

// returns the head of the queue
struct Node* returnHead();

// returns the last node of the queue
struct Node* returnLast();

#endif
