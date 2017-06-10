#include <atomic_ops.h>
#define _GNU_SOURCE
#include <sched.h>


#include "queue.h"
#include <stdio.h>
#include<stdlib.h>
#include "scheduler.h"
#define STACK_SIZE 1024*1024

#undef malloc
#undef free
    void * safe_mem(int op, void * arg) {
      static AO_TS_t spinlock = AO_TS_INITIALIZER;
      void * result = 0;

      spinlock_lock(&spinlock);
      if(op == 0) {
        result = malloc((size_t)arg);
      } else {
        free(arg);
      }
      spinlock_unlock(&spinlock);
      return result;
    }
#define malloc(arg) safe_mem(0, ((void*)(arg)))
#define free(arg) safe_mem(1, arg)
void spinlock_lock(AO_TS_t * lock) {
  while(AO_TS_SET==AO_test_and_set_acquire(lock));
}
void spinlock_unlock(AO_TS_t * lock) {
  AO_CLEAR(lock);
}
void thread_wrap() {
    spinlock_unlock(&ready_list_lock);
    current_thread->initial_function(current_thread->initial_argument);
    current_thread->state =DONE;
    condition_signal(&(current_thread->mycondition));
    yield();
    }

struct queue *ready_list;
// initialize the spinlock
AO_TS_t ready_list_lock = AO_TS_INITIALIZER;
//intial function of kernel thread
int kernel_thread_begin(){
// initialize data structures for a kernel thread here
// it will create an empty thread table entry
struct thread * empty_thread_structure;
empty_thread_structure= malloc(sizeof(struct thread));
empty_thread_structure ->state = RUNNING;
set_current_thread(empty_thread_structure);

while(1){
 
 yield();
}
}

void scheduler_begin(){
	
	set_current_thread(malloc(sizeof(struct thread)));
	current_thread->state = RUNNING;
	ready_list = malloc(sizeof(struct queue));
        mutex_init(&(current_thread->mymutex));
        condition_init(&(current_thread->mycondition));
	ready_list->head =NULL;
	ready_list->tail = NULL;
        /* copied
        clone() creates a new process, in a manner similar to fork(2).
       Unlike fork(2), clone() allows the child process to share parts of
       its execution context with the calling process, such as the memory
       space, the table of file descriptors, and the table of signal
       handlers. */
        int pid;
        char *ptr_top_of_stack = malloc(STACK_SIZE) + STACK_SIZE;
	int x[1] ={0};
        pid = clone(kernel_thread_begin,ptr_top_of_stack, CLONE_THREAD | CLONE_VM | CLONE_SIGHAND,&x[1]);
        
}
// copied from description: This function encapsulates everything necessary to allocate a new thread and then jump to it
struct thread* thread_fork(void(*target)(void*), void * arg){
	//Allocate a new thread control block, and allocate its stack.
   	//Set the new thread's initial argument and initial function.
	struct thread *new_thread;
	new_thread = malloc(sizeof(struct thread));
	//Set the new thread's initial argument and initial function.
  	//printf("in the thread fork\n");
  	new_thread->stack_pointer = malloc(STACK_SIZE) + STACK_SIZE;
	new_thread->initial_argument = arg;
  	new_thread->initial_function = target;
        mutex_init(&(new_thread->mymutex));
        condition_init(&(new_thread->mycondition));
        //lock ready
        spinlock_lock(&ready_list_lock);
  	//Set the current thread's state to READY 
  	current_thread->state = READY;
  	//enqueue it on the ready list.
  	thread_enqueue(ready_list, current_thread);
  	//Set the new thread's state to RUNNING.
  	new_thread->state = RUNNING;
  	//Save a pointer to the current thread in a temporary variable, then set the current thread to the new thread.
	struct thread * temp = current_thread;
    	set_current_thread(new_thread);
    //Call thread_start with the old current thread as old and the new current thread as new.
  	 
    thread_start(temp, current_thread);
    // unlock ready_list_lock
       spinlock_unlock(&ready_list_lock);
	
     return new_thread;
}

void yield() {
      spinlock_lock(&ready_list_lock);
      //If the current thread is not DONE, set its state to READY and enqueue it on the ready list.
    if(current_thread->state != DONE && current_thread->state !=BLOCKED){
         
    	//Set the current thread's state to READY 
  		current_thread->state = READY;
  		//enqueue it on the ready list.
  		thread_enqueue(ready_list, current_thread);
  	}
		struct thread *deqthread;
		// only dequeue if readylist is not empty
		if(!is_empty(ready_list)){
  		//Dequeue the next thread from the ready list and set its state to RUNNING.
                //printf("dequeuing thread\n");
  		deqthread =  thread_dequeue(ready_list);
  		deqthread->state = RUNNING;
		}
               else {
                printf(" ****FATAL READY LIST IS EMPTY *****");
                exit(1);
        }
  		//Save a pointer to the current thread in a temporary variable, then set the current thread to the next thread.
  		struct thread * temp = current_thread;
    		set_current_thread(deqthread);
    	//Call thread_switch with the old current thread as old and the new current thread as new.
  	 
    	thread_switch(temp, current_thread);
 	spinlock_unlock(&ready_list_lock);
  }

void mutex_init(struct mutex *mymutex){
  mymutex->held =0;
  mymutex->lock_holder = NULL;
  mymutex->waiting_threads = malloc(sizeof(struct queue));
  mymutex->waiting_threads->head = NULL;
  mymutex->waiting_threads->tail = NULL;


}
void mutex_lock(struct mutex *mymutex){
  if(mymutex->held ==0){
  mymutex->held =1;
 mymutex->lock_holder = current_thread;
 }
 else if (mymutex->lock_holder==current_thread){
  exit(1);
 }
  else{
  current_thread->state = BLOCKED;
  thread_enqueue(mymutex->waiting_threads, current_thread);
  yield();
}


}

void mutex_unlock(struct mutex *mymutex){
 if(mymutex->lock_holder==current_thread){
 if(is_empty(mymutex->waiting_threads)){
 mymutex->lock_holder = NULL;
 mymutex->held = 0;
}
else{
 struct thread *deq = thread_dequeue(mymutex->waiting_threads);
 deq->state = READY;
 thread_enqueue(ready_list, deq);
 mymutex->lock_holder = deq;
}
}
else{
printf("ERROR ****");
exit(1);
}

}

void condition_init(struct condition  *mycondition){
  
  mycondition->waiting_threads = malloc(sizeof(struct queue));
  mycondition->waiting_threads->head = NULL;
  mycondition->waiting_threads->tail = NULL;


}
void condition_wait(struct condition * mycondition, struct mutex * mymutex){
 if(mymutex->lock_holder != current_thread){
 printf(" current thread does not hed the lock for the condition");
 exit(1);

}
mutex_unlock(mymutex);
current_thread->state = BLOCKED;
thread_enqueue(mycondition->waiting_threads, current_thread);
yield();
}

void condition_signal(struct condition *mycondition){

 if(!is_empty(mycondition->waiting_threads)){
	struct thread *deq = thread_dequeue(mycondition->waiting_threads);
        deq ->state = READY;
        thread_enqueue(ready_list, deq);
}
}
void condition_broadcast(struct condition * mycondition){

  while( ! is_empty(mycondition->waiting_threads)){
    struct thread * deq = thread_dequeue(mycondition->waiting_threads);
    deq ->state = READY;
  thread_enqueue(ready_list, deq);

}
}
void thread_join(struct thread *thr){

   while(thr->state!=DONE){
       mutex_lock(&(thr->mymutex));
       condition_wait(&(thr->mycondition), &(thr->mymutex));
}
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
       printf("freed memory\n");
       
  }
