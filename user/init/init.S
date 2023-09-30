.text
.global _start
_start:
    movq $1, %rax
    movq $1, %rdi
    movq $hello_message, %rsi
    movq $(hello_message_end - hello_message), %rdx
    int $0x80 
.loop:
    jmp .loop

.data 

hello_message:
    .ascii "Hello, world!\n"
hello_message_end:
