.globl swap

    swap:
      pushq %rbx           # callee-save
      movq (%rdi), %rax    # %rax = *x 
      movq (%rsi), %rbx    # %rbx = *y
      movq  %rbx, (%rdi)   # *x = %rbx
      movq  %rax, (%rsi)   # *y = %rax
      popq  %rbx           # callee-restore
      ret                  # return
