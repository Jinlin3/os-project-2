#include "thread_worker_types.h"
#include "thread-worker.h"
#include "linked_list.h"

struct Node* createNode(struct TCB* thread) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = thread;
    newNode->next = NULL;
}

void addToQueue(struct TCB* thread) {
    
}