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

#define STACK_SIZE 16 * 102
#define QUANTUM 5 * 100000 // 0.5s
// #define QUANTUM 10 * 1000

// scheduler id = 0, main id = 1, workers = 2+

// INITIALIZE ALL YOUR OTHER VARIABLES HERE
int init_scheduler_done = 0;

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
    struct TCB* schedulerTCBptr = (struct TCB*)malloc(sizeof(struct TCB));
    if (schedulerTCBptr == NULL) {
        perror("Failed to allocate mainTCB");
        exit(1);
    }
    ucontext_t schedulerContext;
    if (getcontext(&schedulerContext) < 0) {
        perror("getcontext");
        exit(1);
    }
    void* schedulerStack = malloc(STACK_SIZE);
    if (schedulerStack == NULL) { // catching error for malloc
        perror("Failed to allocate stack");
        exit(1);
    }
    schedulerContext.uc_link = NULL;
    schedulerContext.uc_stack.ss_sp = schedulerStack;
    schedulerContext.uc_stack.ss_size = STACK_SIZE;
    schedulerContext.uc_stack.ss_flags = 0;
    makecontext(&schedulerContext,(void*)&schedule, 0); // ERROR HERE PLZ FIX

    schedulerTCBptr->id = 0; // initializing TCB id
    schedulerTCBptr->status = READY; // initializing TCB status
    schedulerTCBptr->priority = 1; // initializing TCB priority to 1 (CHANGE THIS LATER)
    schedulerTCBptr->stack = schedulerStack;
    schedulerTCBptr->context = schedulerContext;

    schedulerTCB = schedulerTCBptr;
}

/* creates main TCB */
int initialize_main() {
    struct TCB* mainTCBptr = (struct TCB*)malloc(sizeof(struct TCB));
    if (mainTCBptr == NULL) {
        perror("Failed to allocate mainTCBptr");
        exit(1);
    }
    ucontext_t mainContext;
    if (getcontext(&mainContext) < 0) {
        perror("getcontext");
        exit(1);
    }
    mainTCBptr->id = 1; // initializing TCB id
    mainTCBptr->status = READY; // initializing TCB status
    mainTCBptr->priority = 1; // initializing TCB priority to 1 (CHANGE THIS LATER)
    mainTCBptr->stack = NULL;
    mainTCBptr->context = mainContext;

    mainTCB = mainTCBptr;
    addToQueue(mainTCB);
}

/* create a new thread */
// create Thread Control Block (TCB)
// create and initialize the context of this worker thread
// allocate space of stack for this thread to run
// after everything is set, push this thread into run queue and
// make it ready for the execution.
int worker_create(worker_t *thread, pthread_attr_t *attr, void *(*function)(void *), void *arg)
{
    printf("worker_create\n");
    // initializes queue, main thread, scheduler thread, and timer
    if (init_scheduler_done == 0) {
        init_scheduler_done++;
    // initializes queue
        createList();
    // initializes scheduler thread
        initialize_scheduler();
    // initializes main thread
        initialize_main();
    // initializes timer
        timer_init();
    }
    // 1 - Create Thread Control Block (TCB)
    struct TCB* workerTCB = (struct TCB*)malloc(sizeof(struct TCB));
    if (workerTCB == NULL) {
        perror("Failed to allocate workerTCB");
        exit(1);
    }
    workerTCB->id = newThreadId(); // initializing TCB id
    workerTCB->status = READY; // initializing TCB status
    workerTCB->priority = 1; // initializing TCB priority to 1 (CHANGE THIS LATER)

    // 2 - create and initialize the context of this worker thread
    ucontext_t workerContext;
    if (getcontext(&workerContext) < 0) {
		perror("getcontext");
		exit(1);
	}
    // allocate space of stack for this thread to run
    void* stack = malloc(STACK_SIZE);
	if (stack == NULL) { // catching error for malloc
		perror("Failed to allocate stack");
		exit(1);
	}
    workerTCB->stack = stack;
    // setting up context
	workerContext.uc_link = NULL;
	workerContext.uc_stack.ss_sp = stack;
	workerContext.uc_stack.ss_size = STACK_SIZE;
	workerContext.uc_stack.ss_flags = 0;

    makecontext(&workerContext, (void*)&function, 1, arg); // pretty sure the function takes in 1 argument
    workerTCB->context = workerContext;

    // push thread into run queue for execution
    addToQueue(workerTCB);
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
    printf("Round-Robin scheduling\n");
    sched_rr();
#else
    // Choose MLFQ
    printf("MLFQ scheduling\n");
    sched_mlfq();
#endif
}

/* starts the timer */
static void timer_init() {
    printf("timer_init\n");
    struct sigaction sa;
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = &signal_handler;
	sigaction (SIGPROF, &sa, NULL);

	timer.it_interval.tv_usec = 0;
	timer.it_interval.tv_sec = 1;
	timer.it_value.tv_usec = 10000; // initial value = 0.01s
	timer.it_value.tv_sec = 0;
    setitimer(ITIMER_PROF, &timer, NULL);
}

/* sigaction function */
static void signal_handler() {
    printf("timer\n");
    swapcontext(&mainTCB->context, &schedulerTCB->context);
}

static void sched_rr()
{
    printf("hello!\n");
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