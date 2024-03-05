#ifndef MTX_TYPES_H
#define MTX_TYPES_H

// contains definitions for data structures needed for implementing threads

#include "thread_worker_types.h"
#include "linked_list.h"

/* mutex struct definition */
typedef struct worker_mutex_t
{
    /* add something here */
    // YOUR CODE HERE
    volatile int lock; // 0 = unlocked, 1 = locked
    struct LinkedList* queue; // blocked queue for all blocked threads waiting to enter the critical section
} worker_mutex_t;

#endif
