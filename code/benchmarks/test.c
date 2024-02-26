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

	printCount();
	printList();

	return 0;
}