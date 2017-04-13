
#include <stdlib.h>
#include <stdio.h>
/* here goes all my declarations in this header file */
/* this is my thread control block representation */
struct thread{
 unsigned char* stack_pointer;
 void (*initial_function)(void*);
 void* initial_argument;
} abcd ; 
/*prototype for factorial and fun_with_threads */
int factorial(int n);
void fun_with_threads(void * arg);
struct thread *current_thread, *inactive_thread;

void thread_switch(struct thread * old, struct thread * new);
void thread_start(struct thread * old, struct thread * new); 
void thread_wrap(void);

void yield();
