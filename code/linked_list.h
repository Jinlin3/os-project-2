#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "thread_worker_types.h"
#include "thread-worker.h"

// contains definitions for data structures needed for implementing linked list

struct Node {
    TCB* thread;
    struct Node* next;
};

#endif
