#include "paging.h"
#include "arch/x86/utils.h"
#include "kinit.h"

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    page_table root_table;
    page_table kernel_l2;
    page_table kernel_l3;

    page_table remap_boot_l2;
    page_table remap_boot_l3;
    page_table remap_boot_l4;

    static void _init_table(uint64_t* entry, uint8_t flags, uint64_t ptr)
    {
        *entry = 0;
        set_bit<0>(*entry, true);
        set_bits<1, 4>(*entry, get_bits<0, 3>(flags) << 1);
        set_bit<63>(*entry, get_bit<4>(flags));
        set_bits<12, 51>(*entry, ptr);
    }

    static void _init_medium(uint64_t* entry, uint8_t flags, uint64_t ptr)
    {
        *entry = 0;
        set_bit<0>(*entry, true); // sets present flag
        set_bits<1, 4>(*entry, get_bits<0, 3>(flags) << 1); // sets the R/W, U/S, PWT, PCD
        set_bit<7>(*entry, true); // sets page size flag
        set_bit<8>(*entry, get_bit<5>(flags)); // sets global flag
        set_bit<12>(*entry, get_bit<6>(flags));
        set_bit<63>(*entry, get_bit<4>(flags)); // sets exec flag
        set_bits<21, 51>(*entry, ptr);
    }

    static void _init_small(uint64_t* entry, uint8_t flags, uint64_t ptr)
    {
        *entry = 0;
        set_bit<0>(*entry, true); // sets present flag
        set_bits<1, 4>(*entry, get_bits<0, 3>(flags) << 1); // sets the R/W, U/S, PWT, PCD
        set_bit<7>(*entry, get_bit<6>(flags)); // sets PAT flag
        set_bit<8>(*entry, get_bit<5>(flags)); // sets global flag
        set_bit<63>(*entry, get_bit<4>(flags)); // sets exec flag
        set_bits<21, 51>(*entry, ptr);
    }

    // no OOP for me today
    void initalize_page_table(uint64_t loaded_pos)
    {
        uint64_t* l1_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(root_table, loaded_pos);
        uint64_t* l2_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l2, loaded_pos);
        uint64_t* l3_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(kernel_l3, loaded_pos);
        auto init_table = GET_PHYSICAL_POINTER_ADDR(_init_table, loaded_pos);
        auto init_medium = GET_PHYSICAL_POINTER_ADDR(_init_medium, loaded_pos);
        auto init_small = GET_PHYSICAL_POINTER_ADDR(_init_small, loaded_pos);

        // write l1 -> l2 entry
        init_table(l1_addr + GET_VIRTUAL_POS(1), 0b00010011, (uint64_t)l2_addr);
        // write l2 -> l3 entry
        init_table(l2_addr + GET_VIRTUAL_POS(2), 0b00010011, (uint64_t)l3_addr);
        // write l3 -> memory entry
        init_medium(l3_addr + GET_VIRTUAL_POS(3), 0b00010011, get_bits<0, 20>(VIRT_LOAD_POSITION));

        uint64_t* l2_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l2, loaded_pos);
        uint64_t* l3_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l3, loaded_pos);
        uint64_t* l4_remap_boot_addr = (uint64_t*)GET_PHYSICAL_POINTER_ADDR(remap_boot_l4, loaded_pos);

        // write l1 -> l2 entry
        init_table(l1_addr + get_page_entry<1>(loaded_pos), 0b00010011, (uint64_t)l2_remap_boot_addr);
        // write l2 -> l3 entry
        init_table(l2_remap_boot_addr + get_page_entry<2>(loaded_pos), 0b00010011, (uint64_t)l3_remap_boot_addr);
        // write l3 -> l4 entry
        init_table(l3_remap_boot_addr + get_page_entry<3>(loaded_pos), 0b00010011, (uint64_t)l4_remap_boot_addr);
        // write l4 -> memory
        init_small(l4_remap_boot_addr + get_page_entry<4>(loaded_pos), 0b00010011, (uint64_t)l2_remap_boot_addr);

        asm volatile(
            "pushq %rax\n"
            "mov %efer, %eax\n"
            "or %eax, 0x100\n"
            "mov %eax, %efer\n"

            "mov %cr4, %eax\n"
            "or %eax, 0x20\n"
            "mov %eax, %cr4\n"

            "mov l1_addr, %rax\n"
            "mov %rax, %cr3\n"

            "mov %cr0, %eax\n"
            "or %eax, 0x80000000\n"
            "mov %eax, %cr0\n"
        );
    }

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, bool override)
    {

    }
}
