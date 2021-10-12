#include "idt/idt.h"
#include "asm/asm_cpp.h"
#include "paging/paging.h"
#include "kinit.h"
#include "config.h"
#include "utils.h"
#include "arch/interface/driver/tty.h"
#include "paging/paging.h"

#define VIDX(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION) >> ((4 - n) * 9 + 12))
#define VIDX_OF(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(loaded_pos) >> ((4 - n) * 9 + 12))
#define INIT_FN inline __attribute__ ((always_inline, section(".place.in.front")))
#pragma GCC diagnostic ignored "-Warray-bounds"

bootloader_packet* PLACE_AT_START packet = nullptr;

bootloader_packet* get_bootloader_packet() { return packet; }
static tty_startup_driver* driver;

class vbe_tty_driver : public tty_startup_driver
{
public:
    void putc(char c)
    {

    }

    ~vbe_tty_driver() { }
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
uint64_t _lpos;

uint64_t tmp_stack[0x80];

static void setup();
__attribute__((section(".text.init"))) void start(const uint64_t loaded_pos)
{
    uint64_t* l1_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos);
    uint64_t* l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l2, loaded_pos);
    uint64_t* l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l3, loaded_pos);

    // write l1 -> l2 entry
    l1_addr[VIDX(1)] = set_ptr(l1_addr[VIDX(1)], l2_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l2 -> l3 entry
    l2_addr[VIDX(2)] = set_ptr(l2_addr[VIDX(2)], l3_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l3 -> memory entry
    l3_addr[VIDX(3)] = set_ptr(l3_addr[VIDX(3)], (void*)loaded_pos, MASK_TABLE_MEDIUM) | RD_WR | PRESENT | PAGE_SIZE;

    l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(identity_l2, loaded_pos);
    l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(identity_l3, loaded_pos);

    // identity map
    // write l1 -> l2 entry
    l1_addr[VIDX_OF(1)] = set_ptr(l1_addr[VIDX_OF(1)], l2_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l2 -> l3 entry
    l2_addr[VIDX_OF(2)] = set_ptr(l2_addr[VIDX_OF(2)], l3_addr, MASK_TABLE_POINTER) | RD_WR | PRESENT;
    // write l3 -> memory entry
    l3_addr[VIDX_OF(3)] = set_ptr(l3_addr[VIDX_OF(3)], (void*)loaded_pos, MASK_TABLE_MEDIUM) | RD_WR | PRESENT | PAGE_SIZE;

    write_cr3((uint64_t)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos));
    setstack((uint64_t)tmp_stack);
    ljmp((void*)setup);

    __builtin_unreachable();
}

static void setup()
{
    // find a nice place for our heap
    
    __builtin_unreachable();
}

void pre_kernel_init()
{
    init_idt();
    register_idt([](uint64_t vec, void* stack) {
        asm volatile("xchg %rbx, %rbx");
    }, 0b11100001, 0x80);
    install_idt();

    asm volatile("int $0x80");

}

void kernel_init()
{

}
