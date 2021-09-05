.section .text.preinit
.code32
    // need 32 bit code w/ 64 bit (buildsystem being bad)
    cmpl $1348561262, %eax
    je .L2
.L3:
    jmp .L3
.L2:
    push %ebx
    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax
    wrmsr
    popl %ebx

    cld
    mov $(4096*4 + 0x100), %ecx
    mov $0x9000, %edi
    xor %eax, %eax
    rep stosb

    // initalize stack
    // write l1->l2
    movl $0x10003, 0x9000
    // write l2->l3
    movl $0x11003, 0x10000
    // write l3->mem
    movl $0x83, 0x11000

    // initalize self -> self
    movl (%ebx), %eax
    push %ebx

    // setup l2->l3 (get index)
    movl %eax, %ebx
    shr $30, %ebx
    // l2->l3 is already present
    jz .L4

    // write l2->l3
    movl $0x12003, 0x10000(,%ebx,8)

    // setup l3->memory (get index)
    mov %eax, %ebx
    shr $21, %ebx
    and $~0xC0000000, %ebx

    mov %eax, %ecx
    orl $0x83, %ecx

    // ebx contains index
    // ecx contains addr to point to

    // write l3->memory
    movl %ecx, 0x12000(,%ebx,8)
    jmp .L5
.L4:
    // setup l3->memory
    mov %eax, %ebx
    shr $21, %ebx
    and $~0xC0000000, %ebx

    mov %eax, %ecx
    orl $0x83, %ecx

    // ebx contains index
    // ecx contains addr to point to

    // write l3->memory
    movl %ecx, 0x11000(,%ebx,8)
.L5:
    push %eax
    mov %cr4, %eax
    or $0x00000020, %eax
    mov %eax, %cr4
    mov $0x9000, %eax
    mov %eax, %cr3
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    pop %eax

    movl $0x00209A00, 0x1300C
    movl $0x00009200, 0x13014
    movw $24, 0x13020
    movl $0x13000, 0x13022
    lgdt 0x13020

    // pad ebx with 32 bits for poping rbx
    pop %ebx
    push $0
    push %ebx

    push $0x8
    addl $0xea, %eax
    push %eax // ugly hack because relocation
    lret
.code64
    pop %rbx
    xor %rax, %rax
    // RBX is location of packet
    movl (%rbx), %eax
.global packet
    add $(packet-0xFFFFFFFF80000000), %rax
    mov %rbx, (%eax)
    // first align stack
    and $~0xF, %rsp

    mov (%rbx), %rdi
    pushq   %rbp
    movq    %rsp, %rbp
    mov (%rbx), %rdi
    call _Z5startm
