/*pushq %reg pushes the value of %reg onto the stack.

popq %reg pops a value off the stack and stores it in %reg.

%rdi and %rsi store the first and second arguments to a function call, respectively.

movq %reg1 %reg2 stores the value of %reg1 in %reg2.

If %reg2 holds a memory address (e.g. a pointer), 
then movq %reg1 (%reg2) stores the 8-byte value of %reg1 at the memory address in %reg2.
If %reg holds a pointer to a C struct, whose fields are 8 bytes wide, the first field's value is at (%reg), the second field's value is at 8(%reg), and so forth.

If %reg holds a C function pointer, jmp *%reg transfers control to the beginning of that function.

If f is the defined name of a C function, jmp f will transfer control to the beginning of that function.

%rax can safely be used as a scratch (temporary) register if needed. */


    .globl thread_start

    thread_start:
    #Push all callee-save registers (%rbx, %rbp, %r12-15) onto the current stack.
      pushq %rbx           # callee-save
      pushq %rbp           
      pushq %r12           
      pushq %r13           
      pushq %r14           
      pushq %r15          
      
      #Save the current stack pointer (%rsp) in old's thread control block.
      movq %rsp, (%rdi)
	  #Load the stack pointer from new's thread control block into %rsp.
	  movq (%rsi), %rsp
	  #Call the initial function of new.
	  
	  jmp thread_wrap
      

