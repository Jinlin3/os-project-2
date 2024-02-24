#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "thread_worker_types.h"

// contains definitions for data structures needed for implementing linked list

struct Node {
    struct TCB* data;
    struct Node* next;
};

#endif
