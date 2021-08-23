.text

.global idt_entries

.global asm_handler_entries

.global idt_start
	cli
	pusha
	movl (asm_handler_entries+0), %eax
	call %eax