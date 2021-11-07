#include "idt/idt.h"
#include "asm/asm_cpp.h"
#include "paging/paging.h"
#include "kinit.h"
#include "init.h"
#include "config.h"
#include "context/context.h"
#include "utils.h"
#include "arch/interface/driver/tty.h"
#include "paging/paging.h"

#define VIDX(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION) >> ((4 - n) * 9 + 12))
#define VIDX_OF(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(loaded_pos) >> ((4 - n) * 9 + 12))
#define INIT_FN inline __attribute__ ((always_inline, section(".place.in.front")))
#pragma GCC diagnostic ignored "-Warray-bounds"

bootloader_packet* PLACE_AT_START packet = nullptr;
bootloader_packet* get_bootloader_packet() { return packet; }

tty_startup_driver* early_tty_driver;

#define PORT 0x3f8          // COM1
class serial_tty_driver : public tty_startup_driver
{
private:
    int is_transmit_empty()
    {
        return inb(PORT + 5) & 0x20;
    }

    void write_serial(char a)
    {
        while (is_transmit_empty() == 0);

        outb(PORT, a);
    }
public:
    serial_tty_driver()
    {
        outb(PORT + 1, 0x00);    // Disable all interrupts
        outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        outb(PORT + 1, 0x00);    //                  (hi byte)
        outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
        outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
        outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

        if(inb(PORT + 0) != 0xAE)
            return;

        outb(PORT + 4, 0x0F);
        puts("Console driver initalized!\n");
    }

    void puts(const char* c)
    {
        MAGIC_BREAK;
        while(*c)
        {
            write_serial(*c);
            c++;
        }
    }
};

namespace paging
{
    extern page_table root_table;
    extern page_table kernel_l2;
    extern page_table kernel_l3;

    extern page_table identity_l2;
    extern page_table identity_l3;
}

using namespace paging;

void main();
static uint64_t _lpos;
static uint64_t tmp_stack[0x100];

static void setup();

__attribute__((section(".text.init"))) void start(const uint64_t loaded_pos)
{
    uint64_t* l1_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos);
    uint64_t* l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l2, loaded_pos);
    uint64_t* l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l3, loaded_pos);

    // write l1 -> l2 entry
    l1_addr[VIDX(1)] = set_ptr(0, l2_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l2 -> l3 entry
    l2_addr[VIDX(2)] = set_ptr(0, l3_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l3 -> memory entry
    l3_addr[VIDX(3)] = set_ptr(0, (void*)loaded_pos, MASK_TABLE_MEDIUM) | RD_WR | PRESENT | PAGE_SIZE;

    l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(identity_l2, loaded_pos);
    l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(identity_l3, loaded_pos);

    // identity map
    // write l1 -> l2 entry
    l1_addr[VIDX_OF(1)] = set_ptr(0, l2_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l2 -> l3 entry
    l2_addr[VIDX_OF(2)] = set_ptr(0, l3_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l3 -> memory entry
    l3_addr[VIDX_OF(3)] = set_ptr(0, (void*)loaded_pos, MASK_TABLE_MEDIUM) | RD_WR | PRESENT | PAGE_SIZE;

    write_cr3((uint64_t)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos));
    setstack((uint64_t)tmp_stack);
    _lpos = loaded_pos;
    ljmp((void*)setup);
    __builtin_unreachable();
}

static void setup()
{
    static serial_tty_driver driver{};
    early_tty_driver = &driver;
    MAGIC_BREAK;
    early_puts("Vtable?");
    early_dbg("Debugging uwu\n");
    pre_kernel_init();
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
    MAGIC_BREAK;
    init_idt();
    early_dbg("Debugging uwu\n");
    early_dbg("Debugging uwu\n");
    early_dbg("Debugging uwu\n");
    early_dbg("Debugging uwu\n");
    early_dbg("Debugging uwu\n");
    early_dbg("Debugging uwu\n");
    early_dbg("Debugging uwu\n");
    register_idt([](uint64_t vec, void* stack) {
        // context switch
        ctx[(int)v].load_ctx(stack);
        v = !v;
        ctx[(int)v].restore_ctx(stack);
    }, 0b11100001, 0x80);
    install_idt();

    asm volatile("int $0x80");
}

void kernel_init()
{

}
