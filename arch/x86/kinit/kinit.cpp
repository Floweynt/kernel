// cSpell:ignore stivale, alignas, rsdp, lapic, efer, wrmsr, kpages, rdmsr, cpuid, kinit, xsdt
#include "boot_resource.h"
#include "kinit/limine.h"
#include <acpi/acpi.h>
#include <asm/asm_cpp.h>
#include <config.h>
#include <cpuid/cpuid.h>
#include <cstdint>
#include <debug/kinit_dump.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <mm/malloc.h>
#include <mm/pmm.h>
#include <new>
#include <paging/paging.h>
#include <pci/pci.h>
#include <printf.h>
#include <smp/smp.h>
#include <sync/spinlock.h>
#include <tty/tty.h>

static limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
    .flags = 0, // no need for x2apic
};

static limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static limine_framebuffer_request framebuffer_request {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

static limine_stack_size_request stack_size_request {
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0,
    .stack_size = paging::PAGE_SMALL_SIZE
};

static limine_kernel_file_request kernel_file_request {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0
};

static limine_kernel_address_request kernel_address_request {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

static limine_rsdp_request rsdp_request {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

alignas(boot_resource) static char buf[sizeof(boot_resource)];
boot_resource& boot_resource::instance() { return *(boot_resource*)(buf); }

boot_resource::boot_resource()
{
    auto mmap_tag = memmap_request.response;
    mmap_length = std::min(mmap_tag->entry_count, MMAP_ENTRIES);

    for (std::size_t i = 0; i < mmap_length; i++)
        mmap_entries[i] = *mmap_tag->entries[i];

    ksize = kernel_file_request.response->kernel_file->size;
    phys_addr = kernel_address_request.response->physical_base;

    root_table = (acpi::rsdp_descriptor*)rsdp_request.response->address;
    auto smp = smp_request.response;
    cores = smp->cpu_count;
    bsp_id_lapic = smp->bsp_lapic_id;
}

static smp::core_local cpu0;
static smp::core_local* cpu0_ptr = &cpu0;

extern "C"
{
    extern std::uint64_t __start_init_array[];
    extern std::uint64_t __end_init_array[];

    // calls the global initialization function from the init_array sections
    // this is required to say, call the constructors of global variables
    static void init_array()
    {
        using init_array_t = void (*)();
        for (std::uint64_t* i = (std::uint64_t*)&__start_init_array; i < (std::uint64_t*)&__end_init_array;
             i++)
            if (*i != 0 && *i != -1ul)
                ((init_array_t)*i)();
    }

    [[noreturn]] void _start()
    {
        init_array();
        new (buf) boot_resource();
        wrmsr(msr::IA32_GS_BASE, (std::uint64_t)&cpu0_ptr);

        mm::init();
        paging::init();
        alloc::init((void*)config::get_val<"mmap.start.heap">,
                    paging::PAGE_SMALL_SIZE * config::get_val<"preallocate-pages">);

        tty::init(framebuffer_request.response);

        debug::print_kinfo();
        std::printf("kinit: _start() started tty\n");
        debug::dump_memory_map();
        cpuid_info::initialize_cpuglobal();
        debug::dump_cpuid_info();

        boot_resource::instance().iterate_xsdt([](const acpi::acpi_sdt_header* entry) {
            paging::map_hhdm_phys(paging::MEDIUM, (std::uint64_t)entry);
            entry = mm::make_virtual<acpi::acpi_sdt_header>((std::uint64_t)entry);
            invlpg((void*)entry);
        });

        debug::dump_acpi_info();

        // PCI time!
        // Note: this should be moved to post-smp init
        pci::scan();

        smp::core_local::create(cpu0_ptr);

        smp::init(smp_request.response);
    }
}
