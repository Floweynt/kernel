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

void main();
uint64_t _lpos;

__attribute__((section(".text.init"))) void start(uint64_t loaded_addr)
{
    // find heap
    __builtin_unreachable();
}

void pre_kernel_init()
{

}

void kernel_init()
{

}
