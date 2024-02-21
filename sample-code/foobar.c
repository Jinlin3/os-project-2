#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <ucontext.h>

#define STACK_SIZE 16 * 1024

ucontext_t fooctx, barctx, scheduler;
int flipflop = 0;

void foo() {
  // setting up timer
  struct sigaction sa;
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = &bar;
	sigaction (SIGPROF, &sa, NULL);

	struct itimerval timer;
	timer.it_interval.tv_usec = 0;
	timer.it_interval.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 1;
  setitimer(ITIMER_PROF, &timer, NULL);

  while (1) {
    printf("foo ");
  }
}

void bar() {
  // setting up timer
  struct sigaction sa;
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = &foo;
	sigaction (SIGPROF, &sa, NULL);

	struct itimerval timer;
	timer.it_interval.tv_usec = 0;
	timer.it_interval.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 1;
  setitimer(ITIMER_PROF, &timer, NULL);

  while (1) {
    printf("bar ");
  }
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

  setcontext(&foo);
  
  while(1);

  return 0;
}