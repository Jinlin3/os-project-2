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
struct LinkedList* createList();

// prints out the number of items in the queue
void printCount(struct LinkedList* list);

// returns the count of the list
int returnCount(struct LinkedList* list);

// prints out queue
void printList(struct LinkedList* list);

// creates a new node and returns it; parameter is a TCB struct
void addToQueue(struct LinkedList* list, struct TCB* tcb);

// pops a node from the list
int pop(struct LinkedList* list, struct TCB* tcb);

// moves first node to the end
int popAndPlop(struct LinkedList* list);

// returns the value of a new thread id
int newThreadId(struct LinkedList* list);

// returns the head of the queue (TCB*)
struct TCB* returnHeadTCB(struct LinkedList* list);

// returns the head of the queue (Node*)
struct Node* returnHeadNode(struct LinkedList* list);

// returns the last node of the queue
struct Node* returnLast(struct LinkedList* list);

// search list for TCB (given thread)
struct TCB* searchTCB(struct LinkedList* list, worker_t thread);

#endif
