## Important Data Structures

LinkedList: To help me throughout the project I created a linked list structure and many helper functions that made my implementation much easier. I used the struct LinkedList to facilitate the run queue and the blocked queue.

TCB: The TCB structure represents a thread and contains its ID, status (ready, running, blocked, exit), context, stack, and a void* exitValuePtr that stores the thread’s exit value (if it has one).

Mutex: The worker_mutex_t structure contains an int lock that can be either 0 (unlocked) or 1 (locked). It also contains a LinkedList that represents the blocked queue for the mutex. Finally, I implemented an int destroyed that represents whether or not the mutex is in use (0) or destroyed (-1).

Run Queue: Similar to mutex’s blocked queue, this was implemented using struct LinkedList.

## Thread and Mutex API Functions

worker_create: This function simply creates a new worker struct TCB. getcontext() and makecontext() are used for the contexts and malloc() for the stacks. Worker IDs are assigned starting from 1 and their statuses are initialized to READY. If the function is running for the first time, it also initializes the run queue, main context (ID = 1000), scheduler context, and calls timer_init() which starts the timer. After this, all contexts created in the function (except for the scheduler) are pushed into the run queue.

worker_yield: It simply uses swapcontext() to save the current thread’s context and switch to the scheduler. Changing the worker state here is unnecessary since all state change logic is handled by the scheduler.

worker_exit: It changes the current thread’s state to EXIT and saves the value_ptr (if provided). It then switches over to the scheduler using setcontext().

worker_join: This function’s main logic revolves around a while loop that prevents the main thread from proceeding until the targeted thread obtains an exit status. For simplicity, I used worker_yield() inside the loop to yield control over to the scheduler. To get the targeted node in the run queue, I used one of my helper functions, searchTCB(), which takes in a worker_t thread and searches for its corresponding node. If a value_ptr is provided, I take the exitValuePtr variable in my TCB struct and save it to the address of value_ptr. Afterwards, I free all dynamically allocated memory such as the context, stack, and TCB struct. Then I pop the node from the run queue. If the last worker thread has been popped, I also free the main thread’s stack and TCB struct.

worker_mutex_init: This function initializes the mutex’s blocked queue, sets the lock to 0, and sets the state to 0 (in use).

worker_mutex_lock: Utilizes __sync_lock_test_and_set to check if the lock is 0 or 1. If locked, it sets the current thread’s state to blocked, pops it from the run queue, pushes it to the blocked queue, and uses worker_yield() to switch to the scheduler. If unlocked, it does nothing.

worker_mutex_unlock: Uses __sync_lock_release to change the lock state to unlocked (0). If the current thread was previously blocked, it changes the state to running, pops it from the blocked queue, and pushes it back to the run queue. In addition, if there are threads in the blocked queue waiting for the mutex, it pops and pushes the thread at the front of the queue back to the run queue with a READY state to make it available for scheduling

worker_mutex_destroy: It checks if the mutex is not in use and if the lock is set to 0. If either of them is not true, it does nothing. Otherwise, it sets mutex->destroyed to -1 and frees the memory allocated for the blocked queue.

## Timer Interrupt

Timer: I used sigaction() and setitimer() similar to the sample code we received in order to set up the timer properly. As for the time quantum, I used 50,000 microseconds for the intervals between the timer interrupts. For sigaction(), I implemented a signal handler that swaps the current context with the scheduler context.

## Scheduler Logic

Here’s the logic of the scheduler step-by-step. If the current thread’s status isn’t EXIT or BLOCKED, I change it to READY since the next context is about to replace the current one. This is important since I don’t want an exited thread to have its state changed to READY. 

From here, I take the head of the run queue and place it at the back of the line. This process is repeated multiple times using a loop until a thread in the queue has a READY state since some of the threads at the front of the queue might have already exited. Once a valid context is at the front of the queue, I use setcontext() to switch the context from the scheduler.

