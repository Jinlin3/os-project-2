CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

all: clean thread-worker.a

thread-worker.a: thread-worker.o linked_list.o
	$(AR) libthread-worker.a thread-worker.o linked_list.o
	$(RANLIB) libthread-worker.a

thread-worker.o: thread-worker.c thread-worker.h thread_worker_types.h mutex_types.h linked_list.h

linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -o $@ $<

ifeq ($(SCHED), RR)
	$(CC) $(CFLAGS) -DRR thread-worker.c
else ifeq ($(SCHED), MLFQ)
	$(CC) $(CFLAGS) -DMLFQ thread-worker.c
else
	echo "no such scheduling algorithm"
endif

clean:
	rm -rf testfile *.o *.a