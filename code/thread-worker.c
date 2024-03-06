// File:	thread-worker.c

// List all group member's name:
/*
Name: Jack Lin
Net ID: jjl327
 */
// username of iLab: iLab1.cs.rutgers.edu
// iLab Server: ILabGPUServers

// contains the skeleton of the worker thread library. All of your implementation goes here.

#include "thread-worker.h"
#include "thread_worker_types.h"
#include "linked_list.h"
#include "mutex_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <ucontext.h>

#define STACK_SIZE 16 * 1024
#define QUANTUM 5 * 10000

// INITIALIZE ALL YOUR OTHER VARIABLES HERE
int init_scheduler_done = 0;
// WORKER ID'S
int workerIdCount = 1;
// SCHEDULER TCB
struct TCB* schedulerTCB;
// MAIN TCB
struct TCB* mainTCB;
// CURRENT TCB
struct TCB* currentTCB;
// timer
struct itimerval timer;
// RUN QUEUE
struct LinkedList* runQueue;

/* creates scheduler TCB */
int initialize_scheduler() {
    schedulerTCB = (struct TCB*)malloc(sizeof(struct TCB));

    ucontext_t* schedulerContext = malloc(sizeof(ucontext_t));
    getcontext(schedulerContext);

    void* schedulerStack = malloc(STACK_SIZE);

    schedulerContext->uc_link = NULL;
    schedulerContext->uc_stack.ss_sp = schedulerStack;
    schedulerContext->uc_stack.ss_size = STACK_SIZE;
    schedulerContext->uc_stack.ss_flags = 0;
    makecontext(schedulerContext, &schedule, 0);

    schedulerTCB->id = 0; // initializing TCB id
    schedulerTCB->status = READY; // initializing TCB status
    schedulerTCB->stack = schedulerStack;
    schedulerTCB->context = schedulerContext;
}

/* create a new thread */
// create Thread Control Block (TCB)
// create and initialize the context of this worker thread
// allocate space of stack for this thread to run
// after everything is set, push this thread into run queue and
// make it ready for the execution.
int worker_create(worker_t *thread, pthread_attr_t *attr, void *(*function)(void *), void *arg)
{
    // initializes queue, main thread, scheduler thread, and timer
    if (init_scheduler_done == 0) {
    // initializes queue
        runQueue = createList();
    // initializes main thread
        mainTCB = (struct TCB*)malloc(sizeof(struct TCB));

        ucontext_t* mainContext = malloc(sizeof(ucontext_t));
        getcontext(mainContext);

        void* mainStack = malloc(STACK_SIZE);

        mainTCB->id = 1000; // initializing TCB id
        mainTCB->status = RUNNING; // initializing TCB status
        mainTCB->stack = mainStack;
        mainTCB->context = mainContext;
        currentTCB = mainTCB;

        addToQueue(runQueue, mainTCB);
    }

    // 1 - Create Thread Control Block (TCB)
    struct TCB* workerTCB = (struct TCB*)malloc(sizeof(struct TCB));

    ucontext_t* workerContext = malloc(sizeof(ucontext_t));
    getcontext(workerContext);

    void* workerStack = malloc(STACK_SIZE);
    workerContext->uc_link = NULL;
    workerContext->uc_stack.ss_sp = workerStack;
    workerContext->uc_stack.ss_size = STACK_SIZE;
    workerContext->uc_stack.ss_flags = 0;
    makecontext(workerContext, function, 1, arg);

    *thread = workerIdCount;
    workerIdCount++;

    workerTCB->id = *thread; // initializing TCB id
    workerTCB->status = READY; // initializing TCB status
    workerTCB->stack = workerStack;
    workerTCB->context = workerContext;

    addToQueue(runQueue, workerTCB);
    getcontext(mainTCB->context);

    // initializes queue, main thread, scheduler thread, and timer
    if (init_scheduler_done == 0) {
        init_scheduler_done++;
    // initializes scheduler thread
        initialize_scheduler();
    // initializes timer
        timer_init();
    }
    return 0;
}

/* give CPU possession to other user-level worker threads voluntarily */
int worker_yield()
{
    // - change worker thread's state from Running to Ready
    // - save context of this thread to its thread control block
    // - switch from thread context to scheduler context
    swapcontext(currentTCB->context, schedulerTCB->context);
    return 0;
};

/* terminate a thread */
void worker_exit(void *value_ptr)
{
    // - if value_ptr is provided, save return value
    // - de-allocate any dynamic memory created when starting this thread (could be done here or elsewhere)
    currentTCB->status = EXIT;
    if (value_ptr != NULL) {
        currentTCB->exitValuePtr = value_ptr;
    }
    
    setcontext(schedulerTCB->context);
}

/* Wait for thread termination */
int worker_join(worker_t thread, void **value_ptr)
{
    // getcontext(mainTCB);
    // - wait for a specific thread to terminate
    // - if value_ptr is provided, retrieve return value from joining thread
    // - de-allocate any dynamic memory created by the joining thread
    struct TCB* targetTCB = searchTCB(runQueue, thread);
    while (targetTCB->status != EXIT) {
        worker_yield();
    }

    if (targetTCB->exitValuePtr != NULL) {
        *value_ptr = targetTCB->exitValuePtr;
    }

    free(targetTCB->stack);
    free(targetTCB->context);
    pop(runQueue, targetTCB); // removes it from queue
    free(targetTCB);

    if (returnCount(runQueue) == 1) {
        free(mainTCB->stack);
        free(mainTCB);
    }

    return 0;
};

/* initialize the mutex lock */
int worker_mutex_init(worker_mutex_t *mutex,
                      const pthread_mutexattr_t *mutexattr)
{
    // initializing mutex structure at address
    struct LinkedList* mutexQueue = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    mutex->lock = 0;
    mutex->queue = mutexQueue;
    mutex->destroyed = INUSE;

    return 0;
};

/* acquire the mutex lock */
int worker_mutex_lock(worker_mutex_t *mutex)
{
    // - use the built-in test-and-set atomic function to test the mutex
    // - if the mutex is acquired successfully, enter the critical section
    // - if acquiring mutex fails, push current thread into block list and
    // context switch to the scheduler thread

    while (__sync_lock_test_and_set(&(mutex->lock), 1)) {
        // pop from run queue and push to blocked queue
        currentTCB->status = BLOCKED;
        addToQueue(mutex->queue, currentTCB);
        pop(runQueue, currentTCB);
        worker_yield();
    }
    return 0;
};

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t *mutex)
{
    // release lock
    __sync_lock_release(&(mutex->lock));

    // if current thread was previously blocked
    if (currentTCB->status == BLOCKED) {
        currentTCB->status = RUNNING;
        pop(mutex->queue, currentTCB);
        addToQueue(runQueue, currentTCB);
    }

    // if there are threads in the queue, put one back into the run queue
    if (returnCount(mutex->queue) > 0) {
        returnHeadTCB(mutex->queue)->status = READY;
        addToQueue(runQueue, returnHeadTCB(mutex->queue));
        pop(mutex->queue, returnHeadTCB(mutex->queue));
    }
    
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
    if (mutex->queue->count != 0 || mutex->lock == 1) {
        return 0;
    }
    mutex->lock = 0;
    mutex->destroyed = DESTROYED;
    free(mutex->queue);
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
    sched_rr();
#else
    // Choose MLFQ
    sched_mlfq();
#endif
}

/* starts the timer */
static void timer_init() {
    struct sigaction sa;
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	if (sigaction(SIGPROF, &sa, NULL) == -1) {
        perror("Error setting up signal handler\n");
        exit(EXIT_FAILURE);
    };

	timer.it_interval.tv_usec = QUANTUM;
	timer.it_interval.tv_sec = 0;
	timer.it_value.tv_usec = QUANTUM;
	timer.it_value.tv_sec = 0;
    if (setitimer(ITIMER_PROF, &timer, NULL) == -1) {
        perror("Error setting up timer\n");
        exit(EXIT_FAILURE);
    };
}

void timer_handler() {
    swapcontext(currentTCB->context, schedulerTCB->context);
}

static void sched_rr()
{
    // - your own implementation of RR
    // (feel free to modify arguments and return types)

    if (currentTCB->status != EXIT && currentTCB->status != BLOCKED) {
        currentTCB->status = READY;
    }
    do {
        popAndPlop(runQueue);
        currentTCB = returnHeadTCB(runQueue);
    } while (currentTCB->status != READY);
    currentTCB->status = RUNNING;
    setcontext(currentTCB->context);
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