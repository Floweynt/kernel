#include "idt/idt.h"
#include "paging/paging.h"
#include "kinit.h"
#include "utils.h"

void main();

__attribute__((section(".basic_paging")))


static bootloader_packet* packet;
bootloader_packet* get_bootloader_packet()
{
    return packet;
}

__attribute__((section(".text.init"))) void start()
{
    uint64_t magic;
    uint64_t _packet;
    asm volatile("mov %rax, magic");
    if(magic != 0x12345678)
        return;
    asm volatile("mov %rbx, packet");
    uint64_t loaded_addr = ((bootloader_packet*)_packet)->loaded_address;
    // after this, we can actually access memory normally
    GET_PHYSICAL_POINTER_ADDR(paging::initalize_page_table, loaded_addr)(loaded_addr);

    packet = (bootloader_packet*)_packet;

    asm volatile("ljmpl $0x8,$main");
    __builtin_unreachable();
}

void pre_kernel_init()
{
    void init_idt();
    void install_idt();
    void register_idt(interrupt_handler handler, uint8_t attrs, size_t num);
}

void kernel_init()
{

}
