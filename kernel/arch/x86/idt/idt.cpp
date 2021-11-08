#include "idt.h"
#include "asm/asm_cpp.h"
#include "interface/driver/tty.h"

static idt_entry idt_entries[256] = {};
uintptr_t idt_handler_entries[256];

extern char idt_entry_start[];
extern char idt_entry_end[];

void init_idt()
{
    uint64_t sizeof_handler = (uint64_t)(idt_entry_end - idt_entry_start);

    for(int i = 0; i < 256; i++)
    {
        uint64_t handler = (uint64_t)idt_entry_start + i * sizeof_handler;
    	idt_entries[i].offset_low = (uint64_t)handler;
        idt_entries[i].offset_mid = (uint64_t)handler >> 16;
    	idt_entries[i].offset_high = ((uint64_t)handler) >> 32;
    }
}

void install_idt()
{
    idt_descriptor descriptor = {
        .size = sizeof(idt_entries),
        .offset = (uint64_t)idt_entries
    };
    asm volatile("lidtq %0" : : "m"(descriptor));
}

inline void putitoa(uint64_t a, uint8_t base)
{
    while(a)
    {
        char buf[2] = {"012345567890abcdef"[a % base], 0};
        print_dbg(buf);
        a = a / base;
    }
}

void register_idt(interrupt_handler handler, size_t num, uint8_t type, uint8_t dpl)
{
    MARKER_BREAK("2");
    LOAD_VARNAME(driver::tty_dvr_startup);
    print_dbg("Registering idt\n");
    MARKER_BREAK("3");
    idt_handler_entries[num] = (uintptr_t)handler;
	idt_entries[num].flags = ((uint16_t)type << 8) | 0x8000 | (type << 13);
    putitoa(idt_entries[num].flags, 16);
    MARKER_BREAK("4");
}
