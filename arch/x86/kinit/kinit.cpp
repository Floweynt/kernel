#include "boot_resource.h"
#include "romfont.h"
#include "stivale2.h"
#include <config.h>
#include <cstdint>
#include <driver/terminal.h>
#include <gdt/gdt.h>
#include <new>
#include <printf.h>
#include MALLOC_IMPL_PATH

static uint8_t stack[8192];

static stivale2_tag unmap_null_tag = {.identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID, .next = 0};

static stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {.identifier = STIVALE2_HEADER_TAG_TERMINAL_ID, .next = (uint64_t)&unmap_null_tag}, .flags = 0};

static stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = (uint64_t)&terminal_hdr_tag},
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0};

static stivale2_header stivale_hdr
    [[gnu::section(".stivale2hdr"), gnu::used]] = {.entry_point = 0,
                                                   .stack = (uintptr_t)stack + sizeof(stack),
                                                   .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
                                                   .tags = (uintptr_t)&framebuffer_hdr_tag};

template <typename H>
H* stivale2_get_tag(struct stivale2_struct* stivale2_struct, uint64_t id)
{
    struct stivale2_tag* current_tag = (stivale2_tag*)stivale2_struct->tags;
    while (true)
    {
        if (current_tag == nullptr)
            return nullptr;
        if (current_tag->identifier == id)
            return (H*)current_tag;
        current_tag = (stivale2_tag*)current_tag->next;
    }
}

alignas(boot_resource) static char buf[sizeof(boot_resource)];
boot_resource& boot_resource::instance() { return *(boot_resource*)(buf); }

boot_resource::boot_resource(stivale2_struct* root)
{
    auto mmap_tag = stivale2_get_tag<stivale2_struct_tag_memmap>(root, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    mmap_length = mmap_tag->entries;
    for (int i = 0; i < mmap_length; i++)
        mmap_entries[i] = mmap_tag->memmap[i];
}

void init_tty(stivale2_struct_tag_framebuffer* buffer)
{
    static driver::simple_tty tmp(*buffer, tty::romfont(8, 16, (void*)font));
    driver::set_tty_startup_driver(&tmp);
}

extern "C"
{
    [[noreturn]] void _start(stivale2_struct* root)
    {
        // basic kernel initalization services
        init_tty(stivale2_get_tag<stivale2_struct_tag_framebuffer>(root, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID));
        new (buf) boot_resource(root);
        boot_resource::instance().iterator_mmap([](const stivale2_mmap_entry& e) {
            if (e.type == 1)
                alloc::add_region((void*)e.base, e.length);
        });

        std::printf("kinit: _start() started tty\n");
        std::printf("memory map:\n");

        boot_resource::instance().iterator_mmap([](const stivale2_mmap_entry& e) {
            switch (e.type)
            {
            case 1:
                std::printf("[\033cg;USABLE      \033]: ");
                break;
            case 2:
                std::printf("[\033cr;RESERVED    \033]: ");
                break;
            case 3:
                std::printf("[\033cy;ACPI RECLAIM\033]: ");
                break;
            case 4:
                std::printf("[\033cy;ACPI NVS    \033]: ");
                break;
            case 5:
                std::printf("[\033cr;BAD         \033]: ");
                break;
            case 1000:
                std::printf("[\033cc;BTL RECLAIM \033]: ");
                break;
            case 1001:
                std::printf("[\033cc;KERN MODULE \033]: ");
                break;
            case 1002:
                std::printf("[\033cc;FRAMEBUFFER \033]: ");
                break;
            default:
                std::printf("[\033cG;UNKNOWN     \033]: ");
            }
            std::printf("0x%016lu-0x%016lu length=0x%016lu\n", e.base, e.base + e.length, e.length);
        });

        new (buf) boot_resource(root);
        gdt::install_gdt();
        while (1)
            __asm__ __volatile__("hlt");
    }
}
