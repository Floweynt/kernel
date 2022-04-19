#include "boot_resource.h"
#include "romfont.h"
#include "stivale2.h"
#include <acpi/acpi.h>
#include <asm/asm_cpp.h>
#include <config.h>
#include <cstdint>
#include <driver/terminal.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <mm/pmm.h>
#include <new>
#include <paging/paging.h>
#include <panic.h>
#include <printf.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

#include MALLOC_IMPL_PATH

static uint8_t stack[4096];

static stivale2_header_tag_smp smp_tag{.tag = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = 0}, .flags = 0};

static stivale2_tag unmap_null_tag = {.identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID, .next = (uint64_t)&smp_tag};

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
    for (std::size_t i = 0; i < mmap_length; i++)
        mmap_entries[i] = mmap_tag->memmap[i];
    ksize = stivale2_get_tag<stivale2_struct_tag_kernel_file_v2>(root, STIVALE2_STRUCT_TAG_KERNEL_FILE_V2_ID)->kernel_size;
    phys_addr = stivale2_get_tag<stivale2_struct_tag_kernel_base_address>(root, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID)
                    ->physical_base_address;
    root_table = (acpi::rsdp_descriptor*)stivale2_get_tag<stivale2_struct_tag_rsdp>(root, STIVALE2_STRUCT_TAG_RSDP_ID)->rsdp;
    auto smp = stivale2_get_tag<stivale2_struct_tag_smp>(root, STIVALE2_STRUCT_TAG_SMP_ID);
    cores = smp->cpu_count;
    bsp_id_lapic = smp->bsp_lapic_id;
}

static void init_tty(stivale2_struct* root)
{
    static driver::simple_tty tmp(
        *stivale2_get_tag<stivale2_struct_tag_framebuffer>(root, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID),
        tty::romfont(8, 8, (void*)font));
    driver::set_tty_startup_driver(&tmp);
}

static void init_paging()
{
    wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));

    std::size_t kpages = std::detail::div_roundup(boot_resource::instance().kernel_size(), 4096ul);

    // workaround for weird stuff
    for (std::size_t i = 0; i < kpages + 10; i++)
    {
        uint64_t vaddr = 0xffffffff80000000 + i * 4096;
        paging::request_page(paging::page_type::SMALL, vaddr, mm::make_physical_kern(vaddr), 0b00010001, true);
    }

    boot_resource::instance().iterate_mmap([](const stivale2_mmap_entry& e) {
        uint64_t current_addr = e.base;
        std::size_t len = e.length / 4069ul;
        uint8_t flags;
        switch (e.type)
        {
        case 1:
        case 3:
        case 0x1000:
        case 0x1002:
            flags = 0b00000001;
            break;
        case 2:
        case 4:
        case 5:
            flags = 0;
            break;
        case 0x1001:
            flags = 0b00010001;
            break;
        default:
            return;
        }

        while (current_addr % 0x200000)
        {
            if (len-- == 0)
                return;
            paging::request_page(paging::page_type::SMALL, mm::make_virtual(current_addr), current_addr, flags);
            current_addr += 4096;
        }

        while ((len >= 0x200) && (current_addr % 0x40000000))
        {
            paging::request_page(paging::page_type::MEDIUM, mm::make_virtual(current_addr), current_addr, flags);
            current_addr += 0x200000;
            len -= 0x200;
        }

        // okay, this is aligned...
        while (len >= 0x40000)
        {
            paging::request_page(paging::page_type::BIG, mm::make_virtual(current_addr), current_addr, flags);
            current_addr += 0x40000000;
            len -= 0x40000;
        }

        while (len >= 0x200)
        {
            paging::request_page(paging::page_type::MEDIUM, mm::make_virtual(current_addr), current_addr, flags);
            current_addr += 0x200000;
            len -= 0x200;
        }

        while (len--)
        {
            paging::request_page(paging::page_type::MEDIUM, mm::make_virtual(current_addr), current_addr, flags);
            current_addr += 0x200000;
        }
    });

    paging::install();
}

static void init_smp(stivale2_struct_tag_smp* smp)
{
    std::printf("init_smp(): bootstrap_processor_id=%u\n", smp->bsp_lapic_id);
    std::printf("  cpus: %lu\n", smp->cpu_count);

    std::size_t index;

    for (std::size_t i = 0; i < smp->cpu_count; i++)
    {
        std::printf("initalizing core: %lu\n", i);
        smp->smp_info[i].extra_argument = i;

        if (smp->smp_info[i].lapic_id == smp->bsp_lapic_id)
        {
            index = i;
            continue;
        }

        smp->smp_info[i].target_stack = (uint64_t)mm::pmm_allocate();
        smp::core_local::get(i).pagemap = (paging::page_table_entry*)mm::pmm_allocate() + 512;
        std::memcpy(smp::core_local::get(i).pagemap + 256, smp::core_local::get(smp->bsp_lapic_id).pagemap + 256,
                    256 * sizeof(paging::page_table_entry));
        smp->smp_info[i].goto_address = (uint64_t)smp::initalize_smp;
    }

    // initalize myself too!
    smp::initalize_smp(&smp->smp_info[index]);
}

namespace alloc::detail
{
    void* start() { return (void*)HEAP_START; }

    std::size_t extend(void* buf, std::size_t n)
    {
        void* d;
        if ((d = mm::pmm_allocate()))
            std::panic("cannot get memory for heap");

        paging::request_page(paging::page_type::SMALL, (uint64_t)buf, (uint64_t)buf, 1);
        return 4096;
    }
} // namespace alloc::detail

extern "C"
{
    [[noreturn]] void _start(stivale2_struct* root)
    {
        new (buf) boot_resource(root);

        boot_resource::instance().iterate_mmap([](const stivale2_mmap_entry& e) {
            if (e.type == 1)
                mm::add_region((void*)(e.base | 0xffff800000000000), e.length / 4096);
        });

        smp::core_local::create();
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(boot_resource::instance().bsp_id()));
        init_tty(root);
        init_paging();
        std::printf("kinit: _start() started tty\n");
        std::printf("memory map:\n");

        boot_resource::instance().iterate_mmap([](const stivale2_mmap_entry& e) {
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

        auto rsdp = boot_resource::instance().rsdp();

        std::printf("ACPI info:\n", rsdp->xsdt_address);
        std::printf("  rsdp data:\n");
        std::printf("    revision=%d\n", (int)rsdp->revision);
        std::printf("    length=%d\n", (int)rsdp->length);

        boot_resource::instance().iterate_xsdt([](const acpi::acpi_sdt_header* entry) {
            paging::request_page(paging::page_type::BIG, mm::make_virtual((uint64_t)entry), (uint64_t)entry, 0);
            entry = mm::make_virtual<acpi::acpi_sdt_header>((uint64_t)entry);
            invlpg((void*)entry);
            std::printf("  entry: (sig=0x%08u)\n", entry->signature);
        });

        init_smp(stivale2_get_tag<stivale2_struct_tag_smp>(root, STIVALE2_STRUCT_TAG_SMP_ID));

        while (1)
            __asm__ __volatile__("hlt");
    }
}
