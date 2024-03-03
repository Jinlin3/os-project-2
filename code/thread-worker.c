// File:	thread-worker.c

// List all group member's name:
/*
 */
// username of iLab:
// iLab Server:

// contains the skeleton of the worker thread library. All of your implementation goes here.

#include "thread-worker.h"
#include "thread_worker_types.h"
#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <ucontext.h>

#define STACK_SIZE 16 * 1024
#define QUANTUM 1 * 100000 // 0.5s
// #define QUANTUM 10 * 1000

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

/* creates scheduler TCB */
int initialize_scheduler() {
    printf("    creating scheduler context\n");
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
    schedulerTCB->priority = 1; // initializing TCB priority to 1 (CHANGE THIS LATER)
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
        createList();
    // initializes main thread
        printf("    creating main context\n");
        mainTCB = (struct TCB*)malloc(sizeof(struct TCB));

        ucontext_t* mainContext = malloc(sizeof(ucontext_t));
        getcontext(mainContext);

        void* mainStack = malloc(STACK_SIZE);

        mainTCB->id = 1000; // initializing TCB id
        mainTCB->status = RUNNING; // initializing TCB status
        mainTCB->priority = 1; // setting main thread id to 1
        mainTCB->stack = mainStack;
        mainTCB->context = mainContext;
        currentTCB = mainTCB;

        addToQueue(mainTCB);
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
    workerTCB->priority = 1; // initializing TCB priority to 1 (CHANGE THIS LATER)
    workerTCB->stack = workerStack;
    workerTCB->context = workerContext;

    printf("    creating worker %d!\n", workerTCB->id);
    addToQueue(workerTCB);
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
    printf("    YIELD FROM: %d\n", currentTCB->id);
    currentTCB->status = READY;
    swapcontext(currentTCB->context, schedulerTCB->context);
    return 0;
};

/* terminate a thread */
void worker_exit(void *value_ptr)
{
    // - if value_ptr is provided, save return value
    // - de-allocate any dynamic memory created when starting this thread (could be done here or elsewhere)
    currentTCB->status = EXIT;
    currentTCB->exitValuePtr = value_ptr;

    setcontext(schedulerTCB->context);
}

/* Wait for thread termination */
int worker_join(worker_t thread, void **value_ptr)
{
    printf("    WORKER JOIN: %d\n", thread);
    // - wait for a specific thread to terminate
    // - if value_ptr is provided, retrieve return value from joining thread
    // - de-allocate any dynamic memory created by the joining thread
    struct TCB* targetTCB = searchTCB(thread);
    targetTCB->joinValuePtr = value_ptr;
    while (targetTCB->status != EXIT) {

    }
    printf("    WORKER JOINED: %d\n", thread);
    printf("    FREEING WORKER %d\n", thread);
    free(currentTCB->stack);
    free(currentTCB->context);
    pop(currentTCB); // removes it from queue
    free(currentTCB);

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
    sched_rr();
#else
    // Choose MLFQ
    sched_mlfq();
#endif
}

/* starts the timer */
static void timer_init() {
    printf("    starting the timer\n");
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
    printf("    RINGRINGRING\n");
    setcontext(schedulerTCB->context);
}

static void sched_rr()
{
    // - your own implementation of RR
    // (feel free to modify arguments and return types)

    printf("    SCHEDULER\n");
    printf("    ROTATING\n");

    if (currentTCB->status != EXIT) {
        currentTCB->status = READY;
    }
    do {
        popAndPlop();
        currentTCB = returnHeadTCB();
    } while (currentTCB->status != READY);
    currentTCB->status = RUNNING;
    printf("    SWITCH TO: THREAD %d\n", currentTCB->id);
    printList();
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