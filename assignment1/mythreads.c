#include <stdio.h>
#include "thread.h"
int factorial(int n) {
    return n == 0 ? 1 : n * factorial(n-1);
  } 

  void fun_with_threads(void * arg) { 
    int i;                                             
    int n = *(int*) arg;
    
    for(i=0;i<7;i++){
  	printf("Control comes to fun_with_threads function\n");
    printf("%d! = %d\n", n+i, factorial(n+i));
	yield(); 
	}
  } 
  void thread_wrap() {
  	printf("Control first come here from the thread_start: inside thread_wrap\n");
    current_thread->initial_function(current_thread->initial_argument);
    yield();
    }
    
  void yield() {
  	printf("inside yield\n");
    struct thread * temp = current_thread;
    current_thread = inactive_thread;
    inactive_thread = temp;
    thread_switch(inactive_thread, current_thread);
  }  
