 # Inline comment
    /* Block comment */

    # void swap(long * x, long * y);void thread_switch(struct thread * old, struct thread * new);

    .globl thread_switch

    thread_switch:
    #Push all callee-save registers (%rbx, %rbp, %r12-15) onto the current stack.
      pushq %rbx           # callee-save
      pushq %rbp           # callee-save
      pushq %r12           # callee-save
      pushq %r13           # callee-save
      pushq %r14           # callee-save
      pushq %r15           # callee-save
      
      #Save the current stack pointer (%rsp) in old's thread control block.
      movq %rsp, (%rdi)
	  #Load the stack pointer from new's thread control block into %rsp.
	  movq (%rsi), %rsp
	  #Pop all callee-save registers from the new stack.
	  popq %r15
	  popq %r14
	  popq %r13
	  popq %r12
	  popq %rbp 
      popq %rbx           # callee-restore
      ret                  # return

