#include "thread_worker_types.h"
#include "thread-worker.h"
#include "linked_list.h"

struct LinkedList* list;

// initializes linked list object
void createList() {
    struct LinkedList* newList = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    if (newList != NULL) {
        newList->head = NULL;
        newList->count = 0;
    }
    list = newList;
}

// prints out the number of items in the queue
void printCount() {
    printf("Number of threads in the queue: %d\n", list->count);
}

// prints out queue
void printList() {
    if (list->count == 0) {
        printf("The queue is empty.\n");
    } else {
        struct Node* ptr = list->head;
        while (ptr != NULL) {
            printf("%d(%d) -> ", ptr->data->id, ptr->data->status);
            ptr = ptr->next;
        }
        printf("END\n");
    }
}

// creates a new node and returns it; parameter is a TCB struct
void addToQueue(struct TCB* thread) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = thread;
    newNode->next = NULL;
    
    if (list->count == 0) {
        list->head = newNode;
    } else {
        struct Node* ptr = list->head;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = newNode;
    }
    list->count++;
}

// moves first node to the end
int popAndPlop() {
    if (list->head == NULL || list->head->next == NULL) {
        return 1;
    }
    struct Node* lastNode = list->head;
    struct Node* firstNode = list->head;
    while(lastNode->next != NULL) {
        lastNode = lastNode->next;
    }
    list->head = list->head->next;
    firstNode->next = NULL;
    lastNode->next = firstNode;
    return 0;
}

// returns the value of a new thread id
int newThreadId() {
    return list->count + 1;
}

// returns the head of the queue
struct TCB* returnHead() {
    return list->head->data;
}

// returns the last node of the queue
struct Node* returnLast() {
    struct Node* ptr = list->head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    return ptr;
}