#include <stdio.h>
#include <unistd.h>
#include "../thread-worker.h"
#include "../linked_list.h"

void* simplef1(void* arg) {
	printf("hello!\n");
}

void* simplef2(void* arg) {
	printf("bye!\n");
}

int main(int argc, char **argv)
{
	int arg1 = 1;
	worker_t f1;
	worker_create(&f1, (void*)&simplef1, NULL, &arg1);

	int arg2 = 2;
	worker_t f2;
	worker_create(&f2, (void*)&simplef2, NULL, &arg2);

	while(1);

	printf("all done!\n");

	return 0;
}