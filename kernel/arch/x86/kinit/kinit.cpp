#include "idt/idt.h"
#include "asm/asm_cpp.h"
#include "paging/paging.h"
#include "kinit.h"
#include "interface/init.h"
#include "config.h"
#include "context/context.h"
#include "utils.h"
#include "kinit_paging.h"
#include "driver/kinit/serial.h"

#define VIDX(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION) >> ((4 - n) * 9 + 12))
#define VIDX_OF(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(loaded_pos) >> ((4 - n) * 9 + 12))
#define INIT_FN inline __attribute__ ((always_inline, section(".place.in.front")))
#pragma GCC diagnostic ignored "-Warray-bounds"

bootloader_packet* PLACE_AT_START packet = nullptr;
bootloader_packet* get_bootloader_packet() { return packet; }

namespace driver
{
    tty_startup_driver* tty_dvr_startup;
}

void main();
static uint64_t _lpos;
static uint64_t tmp_stack[0x100];

static void setup();

__attribute__((section(".text.init"))) void start(const uint64_t loaded_pos)
{
    using namespace paging;

    uint64_t* l1_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos);
    uint64_t* l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l2, loaded_pos);
    uint64_t* l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l3, loaded_pos);
    l1_addr[VIDX(1)] = set_ptr(0, l2_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    l2_addr[VIDX(2)] = set_ptr(0, l3_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    l3_addr[VIDX(3)] = set_ptr(0, (void*)loaded_pos, MASK_TABLE_MEDIUM) | RD_WR | PRESENT | PAGE_SIZE;
    l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(identity_l2, loaded_pos);
    l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(identity_l3, loaded_pos);
    l1_addr[VIDX_OF(1)] = set_ptr(0, l2_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    l2_addr[VIDX_OF(2)] = set_ptr(0, l3_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    l3_addr[VIDX_OF(3)] = set_ptr(0, (void*)loaded_pos, MASK_TABLE_MEDIUM) | RD_WR | PRESENT | PAGE_SIZE;
    write_cr3((uint64_t)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos));
    setstack((uint64_t)tmp_stack);
    _lpos = loaded_pos;
    ljmp((void*)setup);
    __builtin_unreachable();
}

static void setup()
{
    static driver::serial_tty_driver serial;
    serial.init();
    driver::tty_dvr_startup = &serial;
    MARKER_BREAK("1");
    LOAD_VARNAME(driver::tty_dvr_startup);
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
    MARKER_BREAK("1");
    LOAD_VARNAME(driver::tty_dvr_startup);
    init_idt();
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
