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

    PLACE_AT_START page_table stack_l2;
    PLACE_AT_START page_table stack_l3;

    extern page_table remap_boot_l2;
    extern page_table remap_boot_l3;
    extern page_table remap_boot_l4;
}

using namespace paging;

INIT_FN void initalize_page_table(uint64_t loaded_pos)
{
    uint64_t* l1_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos);
    uint64_t* l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l2, loaded_pos);
    uint64_t* l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l3, loaded_pos);

    // write l1 -> l2 entry
    set_table_pointer(*(l1_addr + GET_VIRTUAL_POS(1)), l2_addr, MASK_TABLE_POINTER);
    *(l1_addr + GET_VIRTUAL_POS(1)) = MASK_RW | MASK_PRESENT;

    // write l2 -> l3 entry
    set_table_pointer(*(l2_addr + GET_VIRTUAL_POS(2)), l3_addr, MASK_TABLE_POINTER);
    *(l2_addr + GET_VIRTUAL_POS(2)) =  MASK_RW | MASK_PRESENT;

    // write l3 -> memory entry
    set_table_pointer(*(l3_addr + GET_VIRTUAL_POS(3)), loaded_pos, MASK_TABLE_MEDIUM);
    *(l3_addr + GET_VIRTUAL_POS(3)) =  MASK_RW | MASK_PRESENT;

    uint64_t* l2_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l2, loaded_pos);
    uint64_t* l3_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l3, loaded_pos);
    uint64_t* l4_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l4, loaded_pos);

    if(root_table[0])
    {
        // l1 -> l2 already exists
        if(remap_boot_l2[0])
        {
            // l2->l3 already exists
            if(remap_boot_l3[0])
            {
                // l3->memory exist??? except not really?
                // this should never happend
                while(1) { }
            }

            init_medium(GET_PHYSICAL_POINTER_ADDR(remap_boot_l3, loaded_pos), 0b00000011, 0);
        }
        else
        {
            init_table(GET_PHYSICAL_POINTER_ADDR(remap_boot_l2, loaded_pos), 0b00000011, stack_l3);
            init_medium(GET_PHYSICAL_POINTER_ADDR(stack_l3, loaded_pos), 0b00000011, 0);
        }
    }
    else
    {
        init_table(GET_PHYSICAL_POINTER_ADDR(remap_boot_l2, loaded_pos), 0b00000011, stack_l3);
        init_table(GET_PHYSICAL_POINTER_ADDR(remap_boot_l2, loaded_pos), 0b00000011, stack_l3);
        init_medium(GET_PHYSICAL_POINTER_ADDR(stack_l3, loaded_pos), 0b00000011, 0);
    }

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
