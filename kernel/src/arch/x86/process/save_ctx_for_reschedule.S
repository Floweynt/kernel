.global __save_ctx_for_reschedule
__save_ctx_for_reschedule:
    movq %gs:0, %rax 
    movq (%rax), %rax
    movq $.L0, (%rax)
    
    movq $0, (5 * 8)(%rax)
    movq %rbx, (6 * 8)(%rax)
    movq %rcx, (7 * 8)(%rax)
    movq %rdx, (8 * 8)(%rax)
    movq %rsi, (9 * 8)(%rax)
    movq %rdi, (10 * 8)(%rax)
    movq %r8,  (11 * 8)(%rax)
    movq %r9,  (12 * 8)(%rax)
    movq %r10, (13 * 8)(%rax)
    movq %r11, (14 * 8)(%rax)
    movq %r12, (15 * 8)(%rax)
    movq %r13, (16 * 8)(%rax)
    movq %r14, (17 * 8)(%rax)
    movq %r15, (18 * 8)(%rax)
    movq %rbp, (19 * 8)(%rax)

    pushf
    pop %rbx
    movq %rbx, (2 * 8)(%rax)
    movq %rsp, (3 * 8)(%rax)
    
    movq $1, %rax
.L0:
    ret
