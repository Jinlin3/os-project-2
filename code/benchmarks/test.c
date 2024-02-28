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
	printf("in thread 1!\n");
}

void simplef2(void* arg) {
	printf("in thread 2!\n");
}

int main(int argc, char **argv)
{
    worker_t worker1;
    int arg = 1;
    worker_create(worker1, NULL, &simplef2, &arg);

	printf("main thread end!\n");

    while(1);

	return 0;
}