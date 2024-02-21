// File:	thread-worker.c

// List all group member's name:
/*
 */
// username of iLab:
// iLab Server:

// contains the skeleton of the worker thread library. All of your implementation goes here.

#include "thread-worker.h"
#include "thread_worker_types.h"
#include <ucontext.h>

#define STACK_SIZE 16 * 1024
#define QUANTUM 10 * 1000

// INITIALIZE ALL YOUR OTHER VARIABLES HERE
int init_scheduler_done = 0;

/* create a new thread */
int worker_create(worker_t *thread, pthread_attr_t *attr,
                  void* (*function)(void *), void *arg)
{
    // create Thread Control Block (TCB)
    // create and initialize the context of this worker thread
    // allocate space of stack for this thread to run
    // after everything is set, push this thread into run queue and
    // make it ready for the execution.

    // 1 - Create Thread Control Block (TCB)
    struct TCB* threadPtr = (struct TCB*)malloc(sizeof(struct TCB));
    if (threadPtr == NULL) {
        perror("Failed to allocate threadPtr");
        exit(1);
    }
    threadPtr->id = *thread; // initializing TCB id
    threadPtr->status = READY; // initializing TCB status
    threadPtr->priority = 1; // initializing TCB priority to 1

    // 2 - create and initialize the context of this worker thread
    ucontext_t cctx;
    if (getcontext(&cctx) < 0){
		perror("getcontext");
		exit(1);
	}

    // 3 - allocate space of stack for this thread to run
    void* stack = malloc(STACK_SIZE);
	
	if (stack == NULL) { // catching error for malloc
		perror("Failed to allocate stack");
		exit(1);
	}
	
    // setting up context
	cctx.uc_link = NULL;
	cctx.uc_stack.ss_sp = stack;
	cctx.uc_stack.ss_size = STACK_SIZE;
	cctx.uc_stack.ss_flags = 0;

    makecontext(&cctx, function, 0); // not sure about this one, try implementing it with the function arguments in mind
    
    threadPtr->context = cctx;
    threadPtr->stack = stack;

    // AFTER THIS PUSH THREAD INTO RUN QUEUE FOR EXECUTION

    return 0;
}

/* give CPU possession to other user-level worker threads voluntarily */
int worker_yield()
{

    // - change worker thread's state from Running to Ready
    // - save context of this thread to its thread control block
    // - switch from thread context to scheduler context
    return 0;
};

/* terminate a thread */
void worker_exit(void *value_ptr)
{
    // - if value_ptr is provided, save return value
    // - de-allocate any dynamic memory created when starting this thread (could be done here or elsewhere)
}

/* Wait for thread termination */
int worker_join(worker_t thread, void **value_ptr)
{

    // - wait for a specific thread to terminate
    // - if value_ptr is provided, retrieve return value from joining thread
    // - de-allocate any dynamic memory created by the joining thread
    return 0;
};

/* initialize the mutex lock */
int worker_mutex_init(worker_mutex_t *mutex,
                      const pthread_mutexattr_t *mutexattr)
{
    //- initialize data structures for this mutex
    return 0;
};

/* aquire the mutex lock */
int worker_mutex_lock(worker_mutex_t *mutex)
{

    // - use the built-in test-and-set atomic function to test the mutex
    // - if the mutex is acquired successfully, enter the critical section
    // - if acquiring mutex fails, push current thread into block list and
    // context switch to the scheduler thread
    return 0;
};

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t *mutex)
{
    // - release mutex and make it available again.
    // - put one or more threads in block list to run queue
    // so that they could compete for mutex later.

    return 0;
};

/* destroy the mutex */
int worker_mutex_destroy(worker_mutex_t *mutex)
{
    // - make sure mutex is not being used
    // - de-allocate dynamic memory created in worker_mutex_init

    return 0;
};

/* scheduler */
static void schedule()
{
// - every time a timer interrupt occurs, your worker thread library
// should be contexted switched from a thread context to this
// schedule() function

// - invoke scheduling algorithms according to the policy (RR or MLFQ)

// - schedule policy
#ifndef MLFQ
    // Choose RR

#else
    // Choose MLFQ

#endif
}

static void sched_rr()
{
    // - your own implementation of RR
    // (feel free to modify arguments and return types)
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq()
{
    // - your own implementation of MLFQ
    // (feel free to modify arguments and return types)
}

// Feel free to add any other functions you need.
// You can also create separate files for helper functions, structures, etc.
// But make sure that the Makefile is updated to account for the same.