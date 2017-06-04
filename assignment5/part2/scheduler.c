
#define _GNU_SOURCE
#include <sched.h>
extern void * safe_mem(int, void*);
#define malloc(arg) safe_mem(0, ((void*)(arg)))
#define free(arg) safe_mem(1, arg)

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
/* notes:
source: http://man7.org/linux/man-pages/man2/clone.2.html
CLONE_FILES (since Linux 2.0)
              If CLONE_FILES is set, the calling process and the child
              process share the same file descriptor table.  Any file
              descriptor created by the calling process or by the child
              process is also valid in the other process.  Similarly, if one
              of the processes closes a file descriptor, or changes its
              associated flags (using the fcntl(2) F_SETFD operation), the
              other process is also affected.  If a process sharing a file
              descriptor table calls execve(2), its file descriptor table is
              duplicated (unshared).

              If CLONE_FILES is not set, the child process inherits a copy
              of all file descriptors opened in the calling process at the
              time of clone().  Subsequent operations that open or close
              file descriptors, or change file descriptor flags, performed
              by either the calling process or the child process do not
              affect the other process.  Note, however, that the duplicated
              file descriptors in the child refer to the same open file
              descriptions as the corresponding file descriptors in the
              calling process, and thus share file offsets and file status
              flags (see open(2)).

       CLONE_FS (since Linux 2.0)
              If CLONE_FS is set, the caller and the child process share the
              same filesystem information.  This includes the root of the
              filesystem, the current working directory, and the umask.  Any
              call to chroot(2), chdir(2), or umask(2) performed by the
              calling process or the child process also affects the other
              process.

              If CLONE_FS is not set, the child process works on a copy of
              the filesystem information of the calling process at the time
              of the clone() call.  Calls to chroot(2), chdir(2), umask(2)
              performed later by one of the processes do not affect the
              other process.

       CLONE_IO (since Linux 2.6.25)
              If CLONE_IO is set, then the new process shares an I/O context
              with the calling process.  If this flag is not set, then (as
              with fork(2)) the new process has its own I/O context.

              The I/O context is the I/O scope of the disk scheduler (i.e.,
              what the I/O scheduler uses to model scheduling of a process's
              I/O).  If processes share the same I/O context, they are
              treated as one by the I/O scheduler.  As a consequence, they
              get to share disk time.  For some I/O schedulers, if two
              processes share an I/O context, they will be allowed to
              interleave their disk access.  If several threads are doing
              I/O on behalf of the same process (aio_read(3), for instance),
              they should employ CLONE_IO to get better I/O performance.

              If the kernel is not configured with the CONFIG_BLOCK option,
              this flag is a no-op.
CLONE_THREAD (since Linux 2.4.0-test8)
              If CLONE_THREAD is set, the child is placed in the same thread
              group as the calling process.  To make the remainder of the
              discussion of CLONE_THREAD more readable, the term "thread" is
              used to refer to the processes within a thread group.

              Thread groups were a feature added in Linux 2.4 to support the
              POSIX threads notion of a set of threads that share a single
              PID.  Internally, this shared PID is the so-called thread
              group identifier (TGID) for the thread group.  Since Linux
              2.4, calls to getpid(2) return the TGID of the caller.

              The threads within a group can be distinguished by their
              (system-wide) unique thread IDs (TID).  A new thread's TID is
              available as the function result returned to the caller of
              clone(), and a thread can obtain its own TID using gettid(2).

              When a call is made to clone() without specifying
              CLONE_THREAD, then the resulting thread is placed in a new
              thread group whose TGID is the same as the thread's TID.  This
              thread is the leader of the new thread group.

              A new thread created with CLONE_THREAD has the same parent
              process as the caller of clone() (i.e., like CLONE_PARENT), so
              that calls to getppid(2) return the same value for all of the
              threads in a thread group.  When a CLONE_THREAD thread
              terminates, the thread that created it using clone() is not
              sent a SIGCHLD (or other termination) signal; nor can the
              status of such a thread be obtained using wait(2).  (The
              thread is said to be detached.)

              After all of the threads in a thread group terminate the
              parent process of the thread group is sent a SIGCHLD (or other
              termination) signal.

              If any of the threads in a thread group performs an execve(2),
              then all threads other than the thread group leader are
              terminated, and the new program is executed in the thread
              group leader.

              If one of the threads in a thread group creates a child using
              fork(2), then any thread in the group can wait(2) for that
              child.
CLONE_SIGHAND (since Linux 2.0)
              If CLONE_SIGHAND is set, the calling process and the child
              process share the same table of signal handlers.  If the
              calling process or child process calls sigaction(2) to change
              the behavior associated with a signal, the behavior is changed
              in the other process as well.  However, the calling process
              and child processes still have distinct signal masks and sets
              of pending signals.  So, one of them may block or unblock some
              signals using sigprocmask(2) without affecting the other
              process.

              If CLONE_SIGHAND is not set, the child process inherits a copy
              of the signal handlers of the calling process at the time
              clone() is called.  Calls to sigaction(2) performed later by
              one of the processes have no effect on the other process.

              Since Linux 2.6.0-test6, flags must also include CLONE_VM if
              CLONE_SIGHAND is specified
 CLONE_VM (since Linux 2.0)
              If CLONE_VM is set, the calling process and the child process
              run in the same memory space.  In particular, memory writes
              performed by the calling process or by the child process are
              also visible in the other process.  Moreover, any memory
              mapping or unmapping performed with mmap(2) or munmap(2) by
              the child or calling process also affects the other process.

              If CLONE_VM is not set, the child process runs in a separate
              copy of the memory space of the calling process at the time of
              clone().  Memory writes or file mappings/unmappings performed
              by one of the processes do not affect the other, as with
              fork(2).

*/

void thread_wrap() {
    current_thread->initial_function(current_thread->initial_argument);
    current_thread->state =DONE;
    condition_signal(&(current_thread->mycondition));
    yield();
    }

struct queue *ready_list;

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
     return new_thread;
}

void yield() {
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
