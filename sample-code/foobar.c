#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

#define STACK_SIZE 16 * 1024

ucontext_t fooctx, barctx, scheduler;
int fooBar = 0;

void foo() {
  while (1) {
    printf("foo");
  }
}

void bar() {
  while (1) {
    printf("bar");
  }
}

void schedule() {
  printf("schedule\n");
  printf("foobar: %d\n", fooBar);
  if (fooBar == 0) {
    fooBar = 1;
    swapcontext(&scheduler, &fooctx);
  } else {
    fooBar = 0;
    swapcontext(&scheduler, &barctx);
  }
}

void timer() {
  setcontext(&scheduler);
}

int main() {

  // setting up contexts
  getcontext(&fooctx);
  void* fooStack = malloc(STACK_SIZE);
  fooctx.uc_link=NULL;
	fooctx.uc_stack.ss_sp=fooStack;
	fooctx.uc_stack.ss_size=STACK_SIZE;
	fooctx.uc_stack.ss_flags=0;
  makecontext(&fooctx,(void *)&foo,0);

  getcontext(&barctx);
  void* barStack = malloc(STACK_SIZE);
  barctx.uc_link=NULL;
	barctx.uc_stack.ss_sp=barStack;
	barctx.uc_stack.ss_size=STACK_SIZE;
	barctx.uc_stack.ss_flags=0;
  makecontext(&barctx,(void *)&bar,0);

  getcontext(&scheduler);
  void* schedulerStack = malloc(STACK_SIZE);
  scheduler.uc_link=NULL;
  scheduler.uc_stack.ss_sp=schedulerStack;
  scheduler.uc_stack.ss_size=STACK_SIZE;
  scheduler.uc_stack.ss_flags=0;
  makecontext(&scheduler,(void *)&bar,0);

  struct sigaction sa;
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = &timer;
	sigaction (SIGPROF, &sa, NULL);

	struct itimerval timer;
	timer.it_interval.tv_usec = 0;
	timer.it_interval.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 1;
  setitimer(ITIMER_PROF, &timer, NULL);

  while(1);

  return 0;
}