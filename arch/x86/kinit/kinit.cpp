// cSpell:ignore stivale, alignas, rsdp, lapic, efer, wrmsr, kpages, rdmsr, cpuid, kinit
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
#include <cpuid/cpuid.h>
#include <debug/kinit_dump.h>

#include MALLOC_IMPL_PATH

static uint8_t stack[4096];

static stivale2_header_tag_smp smp_tag{.tag = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = 0}, .flags = 0};

static stivale2_tag unmap_null_tag = {.identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID, .next = (uint64_t)&smp_tag};

static stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = (uint64_t)&unmap_null_tag},
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

        // remember that stack grows down
        smp->smp_info[i].target_stack = (uint64_t)mm::pmm_allocate() + paging::PAGE_SMALL_SIZE;

        smp::core_local::get(i).pagemap = (paging::page_table_entry*)mm::pmm_allocate();

        std::memcpy(smp::core_local::get(i).pagemap + 256, smp::core_local::get(index).pagemap + 256,
                    256 * sizeof(paging::page_table_entry));

        smp->smp_info[i].goto_address = (uint64_t)smp::initialize_smp;
    }

    // initialize myself too!
    smp::initialize_smp(&smp->smp_info[index]);
}

namespace alloc::detail
{
    void* start() { return (void*)HEAP_START; }

    std::size_t extend(void* buf, std::size_t n)
    {
        static uint64_t heap_start = HEAP_START + 4096 * PRE_ALLOCATE_PAGES;;
        if((uint64_t) buf < heap_start)
            return 4096 * std::detail::div_roundup(n, 4096ul);

        std::size_t pages = std::detail::div_roundup(n, 4096ul);

        for(std::size_t i = 0; i < pages; i++)
        {
            void* d;
            if (!(d = mm::pmm_allocate_pre_smp()))
                std::panic("cannot get memory for heap");
            paging::request_page(paging::page_type::SMALL, (uint64_t)buf + i * 4097, mm::make_physical(d));
            invlpg(buf);
        }

        return 4096 * pages;
    }
} // namespace alloc::detail

extern "C"
{
    extern uint64_t __start_init_array[];
    extern uint64_t __end_init_array[];

    static void init_array()
    {
        using init_array_t = void (*)();
        for(uint64_t* i = (uint64_t*) &__start_init_array; i < (uint64_t*) &__end_init_array; i++)
            if(*i != 0 && *i != -1ul)
                ((init_array_t) *i)();
    }

    [[noreturn]] void _start(stivale2_struct* root)
    {
        init_array();

        new (buf) boot_resource(root);

        boot_resource::instance().iterate_mmap([](const stivale2_mmap_entry& e) {
            if (e.type == 1)
                mm::add_region_pre_smp(mm::make_virtual<void>(e.base), e.length / 4096);
        });

        smp::core_local::create();
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(boot_resource::instance().bsp_id()));
        init_tty(root);
        paging::init();
        std::printf("kinit: _start() started tty\n");
        debug::dump_memory_map();

        cpuid_info::initialize_cpuglobal();
        std::printf("cpu_vendor_string: %s\n", cpuid_info::cpu_vendor_string());
        std::printf("cpu_brand_string: %s\n", cpuid_info::cpu_brand_string());

        std::printf("cpu features: ");
        for(std::size_t i = 0; i < CPUID_FEATURE_SIZE * 32; i++)
        {
            if(cpuid_info::test_feature(i) && cpuid_info::FEATURE_STRINGS[i])
                std::printf("%s ", cpuid_info::FEATURE_STRINGS[i]);
        } 
        std::printf("\n");

        auto rsdp = boot_resource::instance().rsdp();
        std::printf("ACPI info:\n", rsdp->xsdt_address);
        std::printf("  rsdp data:\n");
        std::printf("    revision=%d\n", (int)rsdp->revision);
        std::printf("    length=%d\n", (int)rsdp->length);

        boot_resource::instance().iterate_xsdt([](const acpi::acpi_sdt_header* entry) {
            paging::request_page(paging::page_type::MEDIUM, mm::make_virtual((uint64_t)entry), (uint64_t)entry);
            entry = mm::make_virtual<acpi::acpi_sdt_header>((uint64_t)entry);
            invlpg((void*)entry);
            std::printf("  entry: (sig=0x%08u)\n", entry->signature);
        });

        init_smp(stivale2_get_tag<stivale2_struct_tag_smp>(root, STIVALE2_STRUCT_TAG_SMP_ID));

        while (1)
            __asm__ __volatile__("hlt");
    }
}
