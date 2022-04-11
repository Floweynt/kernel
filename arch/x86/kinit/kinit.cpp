#include "boot_resource.h"
#include "romfont.h"
#include "stivale2.h"
#include <config.h>
#include <cstdint>
#include <driver/terminal.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <new>
#include <paging/paging.h>
#include <panic.h>
#include <printf.h>
#include <pmm/pmm.h>
#include <asm/asm_cpp.h>

#include MALLOC_IMPL_PATH

static uint8_t stack[4096];

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
static H* stivale2_get_tag(struct stivale2_struct* stivale2_struct, uint64_t id)
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
    ksize = stivale2_get_tag<stivale2_struct_tag_kernel_file_v2>(root, STIVALE2_STRUCT_TAG_KERNEL_FILE_V2_ID)->kernel_size;
    phys_addr = stivale2_get_tag<stivale2_struct_tag_kernel_base_address>(root, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID)
                    ->physical_base_address;
}

static void init_tty(stivale2_struct* root)
{
    static driver::simple_tty tmp(
        *stivale2_get_tag<stivale2_struct_tag_framebuffer>(root, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID),
        tty::romfont(8, 8, (void*)font));
    driver::set_tty_startup_driver(&tmp);

    // lets PMM::a
}

static void init_paging()
{
    std::size_t kpages = std::detail::div_roundup(boot_resource::instance().kernel_size(), 4096ul);

    for (std::size_t i = 0; i < kpages; i++)
    {
        uint64_t vaddr = 0xffffffff80000000 + i * 4096;
        paging::request_page(paging::page_type::SMALL, vaddr, pmm::make_physical_kern(vaddr), 0b00010001, true);
    }
    
    boot_resource::instance().iterator_mmap([](const stivale2_mmap_entry& e) {
        uint64_t current_addr = e.base;
        std::size_t len = e.length / 4069ul;

        while(current_addr % 0x200000)
        {
            if(len-- == 0)
                return;
            paging::request_page(paging::page_type::SMALL, pmm::make_virtual(current_addr), current_addr, 0b00010001);
            current_addr += 4096;
        }

        while((len >= 0x200) && (current_addr % 0x40000000))
        {
            paging::request_page(paging::page_type::MEDIUM, pmm::make_virtual(current_addr), current_addr, 0b00010001);
            current_addr += 0x200000;
            len -= 0x200;
        }

        // okay, this is aligned...
        while(len >= 0x40000)
        {
            paging::request_page(paging::page_type::BIG, pmm::make_virtual(current_addr), current_addr, 0b00010001);
            current_addr += 0x40000000;
            len -= 0x40000;
        }

        while(len >= 0x200)
        {
            paging::request_page(paging::page_type::MEDIUM, pmm::make_virtual(current_addr), current_addr, 0b00010001);
            current_addr += 0x200000;
            len -= 0x200;
        }

        while(len--)
        {
            paging::request_page(paging::page_type::MEDIUM, pmm::make_virtual(current_addr), current_addr, 0b00010001);
            current_addr += 0x200000;
        }
    });
    paging::install();
}

extern "C"
{
    [[noreturn]] void _start(stivale2_struct* root)
    {
        new (buf) boot_resource(root);

        boot_resource::instance().iterator_mmap([](const stivale2_mmap_entry& e) {
            if(e.type == 1)
                pmm::add_region((void*)(e.base | 0xffff800000000000), e.length / 4096);
        });

        wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));
        init_paging();

        init_tty(root);
        std::printf("kinit: _start() started tty\n");
        std::printf("memory map:\n");

        boot_resource::instance().iterator_mmap([](const stivale2_mmap_entry& e) {
            switch (e.type)
            {
            case 0x1:
                std::printf("[\033cg;USABLE      \033]: ");
                break;
            case 0x2:
                std::printf("[\033cr;RESERVED    \033]: ");
                break;
            case 0x3:
                std::printf("[\033cy;ACPI RECLAIM\033]: ");
                break;
            case 0x4:
                std::printf("[\033cy;ACPI NVS    \033]: ");
                break;
            case 0x5:
                std::printf("[\033cr;BAD         \033]: ");
                break;
            case 0x1000:
                std::printf("[\033cy;BTL RECLAIM \033]: ");
                break;
            case 0x1001:
                std::printf("[\033cc;KERN MODULE \033]: ");
                break;
            case 0x1002:
                std::printf("[\033cc;FRAMEBUFFER \033]: ");
                break;
            default:
                std::printf("[\033cG;UNKNOWN     \033]: ");
            }
            std::printf("0x%016lx-0x%016lx length=0x%016lx\n", e.base, e.base + e.length, e.length);
        });
        
        gdt::install_gdt();
        idt::install_idt();
        
        while (1)
            __asm__ __volatile__("hlt");
    }
}
