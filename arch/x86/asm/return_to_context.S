.global _return_to_context_asm       // [[noreturn]] void _return_to_context_asm(context* ptr: %rdi)
_return_to_context_asm:
    push (4 * 0x8)(%rdi)        // push {ss} 
    push (3 * 0x8)(%rdi)        // push {rsp} 
    push (2 * 0x8)(%rdi)        // push {rflags} 
    push (1 * 0x8)(%rdi)        // push {cs} 
    push (0 * 0x8)(%rdi)        // push {rip} 
    // okay, we then restore the rest of context
    
    movq (0x28 + 0x8 * 0)(%rdi), %rax    
    movq (0x28 + 0x8 * 1)(%rdi), %rbx
    movq (0x28 + 0x8 * 2)(%rdi), %rcx
    movq (0x28 + 0x8 * 3)(%rdi), %rdx
    movq (0x28 + 0x8 * 4)(%rdi), %rsi
    movq (0x28 + 0x8 * 6)(%rdi), %r8
    movq (0x28 + 0x8 * 7)(%rdi), %r9
    movq (0x28 + 0x8 * 8)(%rdi), %r10
    movq (0x28 + 0x8 * 9)(%rdi), %r11
    movq (0x28 + 0x8 * 10)(%rdi), %r12
    movq (0x28 + 0x8 * 11)(%rdi), %r13
    movq (0x28 + 0x8 * 12)(%rdi), %r14
    movq (0x28 + 0x8 * 13)(%rdi), %r15
    movq (0x28 + 0x8 * 14)(%rdi), %rbp

    movq (0x28 + 0x8 * 5)(%rdi), %rdi
    
    iretq
