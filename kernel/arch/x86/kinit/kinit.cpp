#include "idt/idt.h"
#include "paging/paging.h"
#include "kinit.h"
#include "config.h"
#include "utils.h"
#include "driver/tty.h"
#include "paging/paging.h"

#define GET_VIRTUAL_POS(n) (get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION) >> ((4 - n) * 9 + 12))
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

    extern page_table remap_boot_l2;
    extern page_table remap_boot_l3;
    extern page_table remap_boot_l4;
}

using namespace paging;

INIT_FN void init_table(uint64_t* entry, uint8_t flags, uint64_t ptr)
{
    __kprintf((uint64_t)entry);
    *entry = 0;
    set_bit<0>(*entry, true);
    set_bits<1, 4>(*entry, get_bits<0, 3>(flags) << 1);
    set_bit<63>(*entry, get_bit<4>(flags));
    set_bits<12, 51>(*entry, ptr);
}

INIT_FN void init_medium(uint64_t* entry, uint8_t flags, uint64_t ptr)
{
    *entry = 0;
    set_bit<0>(*entry, true); // sets present flag
    set_bits<1, 4>(*entry, get_bits<0, 3>(flags) << 1); // sets the R/W, U/S, PWT, PCD
    set_bit<7>(*entry, true); // sets page size flag
    set_bit<8>(*entry, get_bit<5>(flags)); // sets global flag
    set_bit<12>(*entry, get_bit<6>(flags));
    set_bit<63>(*entry, get_bit<4>(flags));  // sets exec flag
    set_bits<21, 51>(*entry, ptr);
}

INIT_FN void init_small(uint64_t* entry, uint8_t flags, uint64_t ptr)
{
    *entry = 0;
    set_bit<0>(*entry, true); // sets present flag
    set_bits<1, 4>(*entry, get_bits<0, 3>(flags) << 1); // sets the R/W, U/S, PWT, PCD
    set_bit<7>(*entry, get_bit<6>(flags)); // sets PAT flag
    set_bit<8>(*entry, get_bit<5>(flags)); // sets global flag
    set_bit<63>(*entry, get_bit<4>(flags)); // sets exec flag
    set_bits<21, 51>(*entry, ptr);
}

INIT_FN void lcr3(void* virt_addr, uint64_t loaded_addr)
{
    void* phys_addr = GET_PHYSICAL_POINTER_ADDR(virt_addr, loaded_addr);
    asm volatile("mov %0, %%cr3\n\t" : : "r"(phys_addr));
}

INIT_FN void initalize_page_table(uint64_t loaded_pos)
{
    uint64_t* l1_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos);
    uint64_t* l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l2, loaded_pos);
    uint64_t* l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l3, loaded_pos);

    // write l1 -> l2 entry
    init_table(l1_addr + GET_VIRTUAL_POS(1), 0b00000011, (uint64_t)l2_addr);
    // write l2 -> l3 entry
    init_table(l2_addr + GET_VIRTUAL_POS(2), 0b00000011, (uint64_t)l3_addr);
    // write l3 -> memory entry
    init_medium(l3_addr + GET_VIRTUAL_POS(3), 0b00000011, loaded_pos);

    uint64_t* l2_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l2, loaded_pos);
    uint64_t* l3_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l3, loaded_pos);
    uint64_t* l4_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l4, loaded_pos);

    // write l1 -> l2 entry
    init_table(l1_addr + paging::get_page_entry<1>(loaded_pos), 0b00000011, (uint64_t)l2_remap_boot_addr);
    // write l2 -> l3 entry
    init_table(l2_remap_boot_addr + paging::get_page_entry<2>(loaded_pos), 0b00000011, (uint64_t)l3_remap_boot_addr);
    // write l3 -> l4 entry
    init_table(l3_remap_boot_addr + paging::get_page_entry<3>(loaded_pos), 0b00000011, (uint64_t)l4_remap_boot_addr);
    // write l4 -> memory
    init_small(l4_remap_boot_addr + paging::get_page_entry<4>(loaded_pos), 0b00000011, loaded_pos);

    // find a place for our stack
    kend = align<12>(packet->loaded_addr + packet->loaded_size + HEAP_SIZE);

    lcr3(root_table, loaded_pos);
}

void main();
uint64_t _lpos;

__attribute__((section(".text.init"))) void start(uint64_t loaded_addr)
{
    // after this, we can actually access memory normally
    initalize_page_table(loaded_addr);

    // jmp to main
    asm volatile("push $0x8\npush $main\nlretq");
    __builtin_unreachable();
}

void pre_kernel_init()
{

}

void kernel_init()
{

}
