#include "stivale2.h"
#include "boot_resource.h"
#include <cstdint.h>
#include <driver/terminal.h>
#include <gdt/gdt.h>
#include <new>

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
    [[gnu::section("stivale2hdr"), gnu::used]] = {.entry_point = 0,
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
    for(int i = 0; i < mmap_length; i++)
        mmap_entries[i] = mmap_tag->memmap[i];
}

[[noreturn]] void _start(stivale2_struct* root)
{
    auto term_tag = stivale2_get_tag<stivale2_header_tag_terminal>(root, STIVALE2_STRUCT_TAG_TERMINAL_ID);

    if (!term_tag)
        while (1)
            __asm__ __volatile__("hlt");

    static driver::simple_tty tty(term_tag->callback);
    driver::set_tty_startup_driver(&tty);
    gdt::install_gdt();
    new (buf) boot_resource(root);

    while (1)
        __asm__ __volatile__("hlt");
}
