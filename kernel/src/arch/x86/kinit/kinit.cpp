// cSpell:ignore stivale, alignas, rsdp, lapic, efer, wrmsr, kpages, rdmsr, cpuid, kinit, xsdt
#include <acpi/acpi.h>
#include <asm/asm_cpp.h>
#include <config.h>
#include <cpuid/cpuid.h>
#include <cstddef>
#include <cstdint>
#include <debug/debug.h>
#include <debug/kinit_dump.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <kinit/limine.h>
#include <misc/cast.h>
#include <mm/malloc.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <mm/paging/paging_entries.h>
#include <new>
#include <pci/pci.h>
#include <printf.h>
#include <smp/smp.h>
#include <sync/spinlock.h>
#include <tty/tty.h>
#include <type_traits>
#include <utility>

// LIMINE REQUESTS
[[gnu::used]] volatile static const limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
    .flags = 0, // no need for x2apic
};

[[gnu::used]] volatile static const limine_memmap_request memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};

//[[gnu::used]] volatile static limine_framebuffer_request framebuffer_request{
//    .id = LIMINE_FRAMEBUFFER_REQUEST,
//    .revision = 0,
//};

[[gnu::used]] volatile static const limine_stack_size_request stack_size_request{
    .id = LIMINE_STACK_SIZE_REQUEST, .revision = 0, .stack_size = paging::PAGE_SMALL_SIZE};

[[gnu::used]] volatile static const limine_kernel_file_request kernel_file_request{.id = LIMINE_KERNEL_FILE_REQUEST, .revision = 0};

[[gnu::used]] volatile static const limine_kernel_address_request kernel_address_request{.id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

[[gnu::used]] volatile static const limine_rsdp_request rsdp_request{.id = LIMINE_RSDP_REQUEST, .revision = 0};

[[gnu::used]] volatile static const limine_bootloader_info_request btl_info_request{.id = LIMINE_BOOTLOADER_INFO_REQUEST, .revision = 0};

[[gnu::used]] volatile static const limine_module_request module_request{.id = LIMINE_MODULE_REQUEST, .revision = 0};

alignas(boot_resource) static char buf[sizeof(boot_resource)];

auto boot_resource::instance() -> boot_resource& { return *as_ptr<boot_resource>(buf); }

modules::modules()
{
    for (std::size_t i = 0; i < module_request.response->module_count; i++)
    {
        if (!std::strcmp(module_request.response->modules[i]->cmdline, "symbols"))
        {
            symbols = module_request.response->modules[i]->address;
        }
    }
}

boot_resource::boot_resource() : mmap_length(0), pmrs_length(0), mmap_entries(), smp_status(false)
{
    auto* mmap_tag = memmap_request.response;

    if (mmap_tag->entry_count > MMAP_ENTRIES)
    {
        flags = WARN_MMAP_OVERFLOW;
    }

    mmap_length = std::min(mmap_tag->entry_count, MMAP_ENTRIES);

    for (std::size_t i = 0; i < mmap_length; i++)
    {
        mmap_entries[i] = *mmap_tag->entries[i];
    }

    ksize = kernel_file_request.response->kernel_file->size;
    phys_addr = kernel_address_request.response->physical_base;

    root_table = as_ptr(rsdp_request.response->address);
    auto* smp = smp_request.response;
    cores = smp->cpu_count;
    bsp_id_lapic = smp->bsp_lapic_id;
}

namespace
{
    smp::core_local cpu0;
    smp::core_local* cpu0_ptr = &cpu0;

    void handle_init_warnings()
    {
        static constexpr std::pair<std::uint32_t, const char*> flags[] = {{WARN_MMAP_OVERFLOW, "mmap_overflow"}, {WARN_PMM_OVERFLOW, "pmm_overflow"}};
        boot_resource& instance = boot_resource::instance();
        if (instance.warn_init())
        {
            std::printf("WARN kinit: ");
            for (const auto& flag : flags)
            {
                if (instance.warn_init() | flag.first)
                {
                    std::printf("%s", flag.second); // input is sanitized
                }
            }

            std::printf("\n");
        }
    }
} // namespace

extern "C"
{
    extern std::uint64_t __start_init_array[];
    extern std::uint64_t __end_init_array[];

    // calls the global initialization function from the init_array sections
    // this is required to say, call the constructors of global variables
    namespace
    {
        void init_array()
        {
            for (std::uintptr_t* i = as_ptr(&__start_init_array); i < as_ptr(&__end_init_array); i++)
            {
                if (*i != 0 && *i != -1UL)
                {
                    as_ptr<void()> (*i)();
                }
            }
        }
    } // namespace

    [[noreturn]] void _start()
    {
        // really early init functions...
        init_array();
        new (decay_arr(buf)) boot_resource();
        boot_resource& instance = boot_resource::instance();

        // set up gs base for code-local access
        wrmsr(msr::IA32_GS_BASE, as_uptr(&cpu0_ptr));

        // okay, set up core functionality to hopefully get useful information out of the kernel
        mm::init();
        tty::init();

        // debugging information, not important for booting
        std::printf("kinit: _start() started tty\n");
        std::printf("booted from: %s-v%s\n", btl_info_request.response->name, btl_info_request.response->version);
        debug::print_kinfo();
        debug::dump_memory_map();
        cpuid_info::initialize_cpuglobal();
        debug::dump_cpuid_info();

        instance.iterate_xsdt([](const acpi::acpi_sdt_header* entry) {
            paging::map_hhdm_page(paging::MEDIUM, as_uptr(entry));
            entry = mm::make_virtual<acpi::acpi_sdt_header>(as_uptr(entry));
            invlpg((void*)entry);
        });

        debug::dump_acpi_info();
        pci::scan();

        smp::core_local::create(cpu0_ptr);
        handle_init_warnings();
        smp::init(smp_request.response);
    }
}
