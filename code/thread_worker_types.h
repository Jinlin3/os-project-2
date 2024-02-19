#ifndef TW_TYPES_H
#define TW_TYPES_H

// contains definitions for data structures needed for implementing threads

#include <ucontext.h>

typedef unsigned int worker_t;

typedef enum {
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} thread_status;

typedef struct TCB {
    /* add important states in a thread control block */
    worker_t id; // thread Id
    thread_status status;  // thread status
    ucontext_t context; // thread context
    void* stack; // thread stack
    int priority; // thread priority
    // And more ...

    // YOUR CODE HERE

} tcb;

#endif
