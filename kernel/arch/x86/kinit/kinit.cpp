#include "idt/idt.h"
#include "gdt/gdt.h"
#include "asm/asm_cpp.h"
#include "paging/paging.h"
#include "kinit.h"
#include "interface/init.h"
#include "config.h"
#include "context/context.h"
#include "utils.h"
#include "kinit_paging.h"
#include "driver/kinit/serial.h"
#include "stivale2.h"
 
static uint8_t stack[8192];

static stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};
 
static stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};
 
static stivale2_header stivale_hdr [[gnu::section("stivale2hdr"), gnu::used]] = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

template<typename H>
H* stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) 
{
    struct stivale2_tag *current_tag = (stivale2_tag*) stivale2_struct->tags;
    while(true)
    {
        if (current_tag == nullptr)
            return nullptr;
        if (current_tag->identifier == id)
            return (H*) current_tag;
        current_tag = (stivale2_tag*)current_tag->next;
    }
}
 
[[noreturn]] void _start(stivale2_struct *stivale2_struct) 
{
    auto term_tag = stivale2_get_tag<stivale2_header_tag_terminal>(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
 
    if (!term_tag) 
        while(1) __asm__ __volatile__("hlt");
 
    // Let's get the address of the terminal write function.
    void *term_write_ptr = (void *)term_str_tag->term_write;
 
    // Now, let's assign this pointer to a function pointer which
    // matches the prototype described in the stivale2 specification for
    // the stivale2_term_write function.
    void (*term_write)(const char *string, size_t length) = term_write_ptr;
 
    // We should now be able to call the above function pointer to print out
    // a simple "Hello World" to screen.
    term_write("Hello World", 11);
 
    // We're done, just hang...
    for (;;) {
        asm ("hlt");
    }
}


static void setup()
{
    static driver::serial_tty_driver serial;
    serial.init();
    driver::tty_dvr_startup = &serial;
    pre_kernel_init();

    // mark function as unreachable, because pre_kernel_init never returns
    __builtin_unreachable();
}

using task = void (*)();

task tasks[2] = {
    []() {
        while(1)
        {
            asm volatile(
                "mov $12, %rax\n"
                "mov $13, %rbx\n"
                "mov $14, %rcx\n"
                "mov $15, %rdx\n"
                "mov $16, %rsi\n"
                "mov $17, %rdi\n"
                "mov $18, %r8\n"
                "mov $19, %r9\n"
                "mov $20, %r10\n"
                "mov $21, %r11\n"
                "mov $22, %r12\n"
                "mov $23, %r13\n"
                "mov $24, %r14\n"
                "mov $25, %r15\n"
                "int $0x80"
            );
            asm volatile("xchg %rbx, %rbx");
        }
    },
    []() {
        while(1)
        {
            asm volatile(
                "mov $112, %rax\n"
                "mov $113, %rbx\n"
                "mov $114, %rcx\n"
                "mov $115, %rdx\n"
                "mov $116, %rsi\n"
                "mov $117, %rdi\n"
                "mov $118, %r8\n"
                "mov $119, %r9\n"
                "mov $120, %r10\n"
                "mov $121, %r11\n"
                "mov $122, %r12\n"
                "mov $123, %r13\n"
                "mov $124, %r14\n"
                "mov $125, %r15\n"
                "int $0x80"
            );
            asm volatile("xchg %rbx, %rbx");
        }
    }
};

context ctx[2] = {
    {
        .rip = (uint64_t)tasks[0]
    },
    {
        .rip = (uint64_t)tasks[1]
    }
};

bool v = false;

void pre_kernel_init()
{
    install_gdt();
    init_idt();
    register_idt([](uint64_t vec, void* stack) {
        // context switch
        ctx[(int)v].load_ctx(stack);
        v = !v;
        ctx[(int)v].restore_ctx(stack);
    }, 0x80);
    install_idt();
    MAGIC_BREAK;
    asm volatile("int $0x80");
}

void kernel_init()
{

}
