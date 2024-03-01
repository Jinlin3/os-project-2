#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <ucontext.h>

#include "../thread-worker.h"
#include "../linked_list.h"

void simplef1(void* arg) {
    printList();
    printf("simplef1!\n");
}

void simplef2(void* arg) {
    printList();
	printf("simplef2!\n");
}

int main(int argc, char **argv)
{
    worker_t worker1;
    int arg1 = 1;
    worker_create(worker1, NULL, &simplef1, &arg1);

    worker_t worker2;
    int arg2 = 2;
    worker_create(worker2, NULL, &simplef2, &arg2);

    worker_yield();

	printf("main thread end!\n");

    while(1);

	return 0;
}