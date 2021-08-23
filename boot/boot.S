.code16
.code16gcc
    // set up registers
	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %ss
    mov %ax, %es

	mov %dl, 0x1000

    mov $0x7c00, %ax
    mov %ax, %sp

    mov $0x02, %ah
    mov $0x8, %al
    mov $0x8000, %bx
    mov $0x0002, %cx
    mov $0x0, %dh
    int $0x13
    ljmp $0x0,$0x8000

    /*
    // switch to protected mode
    cli
    pushw %ds
    pushw %es

	// enable A20
	in $0x92, %al
	or $2, %al
	out %al, $0x92

	// switch to unreal mode
	lgdtw gdtr
	mov %cr0, %eax
	or $1, %al
	mov %eax, %cr0
    jmp pmode

pmode:
    // finish switching
	mov $0x10, %bx
	mov %bx, %ds
	mov %bx, %es
	and $0xfe, %al
	mov %eax, %cr0
    jmp unrealmode
unrealmode:
	popw %es
	popw %ds

    mov  $0, %al
    movl $3840, %ecx
    movl $0xb8000, %edi
    addr32 rep stosb

setup_video:
    mov $0x4f00, %ax
    mov $0x2000, %di
    int $0x10
    // iterate thru all video ops
    pushw %es
    movw 0x200E, %bx
.L3:
    movw 0x2010, %es
    cmpw $0xffff, %es:(%bx)
    je .L5
    mov $0x4F01, %ax
    mov %es:(%bx), %cx
    mov $0x3000, %di
    xor %dx, %dx
    mov %dx, %es
    int $0x10
    add $2, %bx
    // we want at least 642x480
    cmpw $642, 0x3012
    jl .L3
    cmpw $480, 0x3014
    jl .L3

    mov 0x3000, %dx
    and $(1 << 7), %dx
    jnz .L4

    jmp .L3
.L4:
    movb $0x01, 0x4000
    mov %cx, 0x4001
    movl $0x2000, 0x4003
    movl $0x3000, 0x4007

    mov $0x4f02, %ax
    mov %cx, %bx
    // flags
    or $(1 << 14), %bx
    popw %es
    int $0x10

    jmp load_kernel
.L5:
    movb $0x00, 0x4000
    popw %es

load_kernel:
	movl $0x100000, %ebx
	movl $0x1000, %edx
    call load

    // enable A20
    lgdtw gdtr
    mov %cr0, %eax
	or $1, %eax
	mov %eax, %cr0

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

    // jump to kernel
    ljmpl $0x8,$0x100000

halt:
    jmp halt

// BX should contain offset to load at
// DX should contain sectors to load
load:
    xor %eax, %eax
.L2:
    // while (eax < edx)
    cmp %edx, %eax
    jge .L1
    inc %eax

    push %eax
    push %edx
    push %ebx
    mov $0x42, %ah
    mov $dap, %si
    mov boot_disk, %dl
    int $0x13
    pop %ebx
    pop %edx
    pop %eax

    // data is loaded at 0x1000 to ebx
    movl $0x1000, %esi
    movl %ebx, %edi
    xor %ecx, %ecx
    mov $512, %cx
    addr32 rep movsb

    // add one to dap_lba_low (go to next sector)
    incl dap_lba_low
	add $512, %ebx

    jmp .L2
.L1:
    ret

.align 8
gdtr:
	.word gdt_end - gdt_base - 1
	.long gdt_base

gdt_base:
gdt_null:
    .quad 0
gdt_code:
    .word 0xffff
    .word 0
    .byte 0
    .byte 0x9a
    .byte 0xcf
    .byte 0
gdt_data:
    .word 0xffff
    .word 0
    .byte 0
    .byte 0x92
    .byte 0xcf
    .byte 0
gdt_end:

.global dap
dap:
	.byte 16
	.byte 0 /* always 0

.global dap_sectors
dap_sectors:
	.word 1
.global dap_buffer
dap_buffer:
	.long 0x1000
dap_lba_low:
	.long 1
dap_lba_high:
	.long 0
*/
