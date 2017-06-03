/*scheduler header file*/
 typedef enum {
    RUNNING, // The thread is currently running.
    READY,   // The thread is not running, but is runnable.
    BLOCKED, // The thread is not running, and not runnable.
    DONE     // The thread has finished.
  } state_t;
 struct mutex{
  int held;
  struct queue *waiting_threads;
  struct thread *lock_holder;
};
struct condition {
struct queue *waiting_threads;
};

void mutex_init(struct mutex*);
void mutex_lock(struct mutex*);
void mutex_unlock(struct mutex*);

void condition_init(struct condition *);
void condition_wait(struct condition *, struct mutex *);
void condition_signal(struct condition *);
void condition_broadcast(struct condition *); 
 struct thread{
 unsigned char* stack_pointer;
 void (*initial_function)(void*);
 void* initial_argument;
 state_t state;
 struct condition mycondition;
 struct mutex mymutex;
};

extern struct thread * get_current_thread();
extern void set_current_thread(struct thread*);

void scheduler_begin();
void thread_start(struct thread *old, struct thread *new);
void thread_switch(struct thread *old, struct thread *new);

struct thread * thread_fork(void(*target)(void*), void * arg);
void thread_join(struct thread *);
void yield();
void scheduler_end();
extern struct thread * current_thread, *inactive_thread;
extern struct thread * get_current_thread();
extern void set_current_thread(struct thread*);
#define current_thread (get_current_thread())


