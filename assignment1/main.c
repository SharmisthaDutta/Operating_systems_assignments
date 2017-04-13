
#include <stdio.h>
#include "thread.h"
#define STACK_SIZE 1024*1024
int main()
{
	
	printf("here in main function\n");
	int i;
	current_thread = malloc(sizeof(abcd));
	
	inactive_thread = malloc(sizeof(abcd));
	
	int * p = malloc(sizeof(int));
  		*p = 5;
  	current_thread->initial_argument = p;
  	//current_thread->stack_pointer = malloc(1024*1024) + (1024*1024);
  	current_thread->stack_pointer = malloc(STACK_SIZE) + STACK_SIZE;
  	current_thread->initial_function = fun_with_threads;
  	 
    thread_start(inactive_thread, current_thread);
    //for(i =0;i<5;i++){
    //	printf("looping\n ");
    //	yield();
    //}
	return 0;
}
