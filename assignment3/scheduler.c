#include "scheduler.h"
#include "queue.h"
#include <stdio.h>
#include<stdlib.h>
#define STACK_SIZE 1024*1024



void thread_wrap() {
  	printf("Control first come here from the thread_start: inside thread_wrap\n");
    current_thread->initial_function(current_thread->initial_argument);
    // if we dont put state as done scheduler never terminates
    current_thread->state =DONE;
   // this is a crazy coding. first i had segmentation fault to correct that i have this infinite loop which never end my code
    while(1==1)
    yield();
    }

struct queue *ready_list;
struct thread * current_thread;
void scheduler_begin(){
	//int i;
	current_thread = malloc(sizeof(struct thread));
	current_thread->state = RUNNING;
	ready_list = malloc(sizeof(struct queue));
	ready_list->head =NULL;
	ready_list->tail = NULL;
}
// copied from description: This function encapsulates everything necessary to allocate a new thread and then jump to it
void thread_fork(void(*target)(void*), void * arg){
	//Allocate a new thread control block, and allocate its stack.
   	//Set the new thread's initial argument and initial function.
	struct thread *new_thread;
	new_thread = malloc(sizeof(struct thread));
	//Set the new thread's initial argument and initial function.
  	//printf("in the thread fork\n");
  	new_thread->stack_pointer = malloc(STACK_SIZE) + STACK_SIZE;
	new_thread->initial_argument = arg;
  	new_thread->initial_function = target;
  	//Set the current thread's state to READY 
  	current_thread->state = READY;
  	//enqueue it on the ready list.
  	thread_enqueue(ready_list, current_thread);
  	//Set the new thread's state to RUNNING.
  	new_thread->state = RUNNING;
  	//Save a pointer to the current thread in a temporary variable, then set the current thread to the new thread.
	struct thread * temp = current_thread;
    	current_thread = new_thread;
    //Call thread_start with the old current thread as old and the new current thread as new.
  	 
    thread_start(temp, current_thread);	
}
// yield
void yield() {
  	//printf("inside yield\n");
    //If the current thread is not DONE, set its state to READY and enqueue it on the ready list.
    if(current_thread->state != DONE){
         //printf("inside the non done state of thread\n");
    	//Set the current thread's state to READY 
  		current_thread->state = READY;
  		//enqueue it on the ready list.
  		thread_enqueue(ready_list, current_thread);
  	}
		struct thread *deqthread;
		// only dequeue if readylist is not empty
		if(!is_empty(ready_list)){
  		//Dequeue the next thread from the ready list and set its state to RUNNING.
                //printf("dequeuing the next thread\n");
  		deqthread =  thread_dequeue(ready_list);
  		deqthread->state = RUNNING;
		}
  		//Save a pointer to the current thread in a temporary variable, then set the current thread to the next thread.
  		struct thread * temp = current_thread;
    		current_thread = deqthread;
    	//Call thread_switch with the old current thread as old and the new current thread as new.
  	 
    	thread_switch(temp, current_thread);
 
  }  
  void scheduler_end(){
  	printf("I am inside scheduler end function now\n");
  	while(!is_empty(ready_list)){
                // and incur segmentation fault :(
  		//printf("not terminating lets yield again\n");
                yield();
  	}
      
        free(ready_list);
        free(current_thread);
        //printf("freed memory");
       
  }