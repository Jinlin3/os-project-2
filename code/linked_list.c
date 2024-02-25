#include "thread_worker_types.h"
#include "thread-worker.h"
#include "linked_list.h"

struct LinkedList* list;

// initializes linked list object
void* createList() {
    struct LinkedList* newList = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    if (newList != NULL) {
        newList->head = NULL;
        newList->count = 0;
    }
    list = newList;
}

// prints out queue
void* printList() {
    if (list->count == 0) {
        printf("The queue is empty.\n");
    } else {
        struct Node* ptr = list->head;
        do {
            printf("%d -> ", ptr->data->id);
        } while(ptr->next != NULL);
        printf("END OF QUEUE\n");
    }
}

// creates a new node and returns it; parameter is a TCB struct
void* addToQueue(struct TCB* thread) {
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