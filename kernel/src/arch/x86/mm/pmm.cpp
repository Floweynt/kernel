#include <mm/pmm.h>
#include <config.h>
#include <cstdint>
#include <kinit/limine.h>
#include <klog/klog.h>
#include <paging/paging.h>
#include <sync/spinlock.h>
#include <utils/id_allocator.h>

namespace mm
{
    inline constexpr auto PMM_COUNT = config::get_val<"pmm-count">;
    namespace
    {
        std::array<pmm_region, PMM_COUNT> region;
        id_allocator<PMM_COUNT> meta_allocator;
        lock::spinlock pmm_alloc_lock;
    } // namespace

    void init()
    {
        std::size_t count = 0;
        boot_resource::instance().iterate_mmap([&](const limine_memmap_entry& memmap) {
            if (memmap.type == LIMINE_MEMMAP_USABLE)
            {
                if (count == PMM_COUNT)
                {
                    boot_resource::instance().warn_init(WARN_PMM_OVERFLOW);
                    return;
                }
                mm::add_region(mm::make_virtual(memmap.base), memmap.length / paging::PAGE_SMALL_SIZE);
                count++;
            }
        });
    }

    void add_region(std::uintptr_t start, std::size_t len)
    {
        lock::spinlock_guard g(pmm_alloc_lock);
        auto index = meta_allocator.allocate();

        if (index == -1UL)
        {
            klog::panic("pmm buffer exhausted");
        }

        region[index] = pmm_region(start, len);
    }

    auto pmm_allocate(std::size_t len) -> void*
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard guard(pmm_alloc_lock);
            if (region[i].exists())
            {
                auto ptr = region[i].allocate(len);
                if (ptr != 0)
                {
                    return std::memset((void*)ptr, 0, paging::PAGE_SMALL_SIZE * len);
                }
            }
        }

        return nullptr;
    }
} // namespace mm
