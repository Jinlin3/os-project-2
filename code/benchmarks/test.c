#include <stdio.h>
#include <unistd.h>
#include "../thread-worker.h"
#include "../linked_list.h"

void* simplef() {
	printf("hello!\n");
}

int main(int argc, char **argv)
{
	void* arg1 = NULL;
	worker_t f1;
	worker_create(&f1, (void*)&simplef, NULL, &arg1);

	void* arg2 = NULL;
	worker_t f2;
	worker_create(&f2, (void*)&simplef, NULL, &arg2);

	void* arg3 = NULL;
	worker_t f3;
	worker_create(&f3, (void*)&simplef, NULL, &arg3);

	void* arg4 = NULL;
	worker_t f4;
	worker_create(&f4, (void*)&simplef, NULL, &arg4);

	printCount();
	printList();

	return 0;
}