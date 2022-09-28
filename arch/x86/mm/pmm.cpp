#include "pmm.h"
#include <klog/klog.h>
#include <kinit/limine.h>
#include <config.h>
#include <cstdint>
#include <paging/paging.h>
#include <sync/spinlock.h>
#include <utils/id_allocator.h>

namespace mm
{
    inline constexpr auto PMM_COUNT = config::get_val<"pmm-count">;
    static pmm_region region[PMM_COUNT];
    static id_allocator<PMM_COUNT> meta_allocator;
    static lock::spinlock pmm_alloc_lock;

    void init()
    {
        std::size_t count = 0;
        boot_resource::instance().iterate_mmap([&](const limine_memmap_entry& e) {
            if (e.type == LIMINE_MEMMAP_USABLE) {
                if(count == PMM_COUNT)
                {
                    boot_resource::instance().warn_init(WARN_PMM_OVERFLOW);
                    return;
                } 
                mm::add_region(mm::make_virtual(e.base), e.length / paging::PAGE_SMALL_SIZE);
                count++;

            }
        });
    }

    void add_region(std::uintptr_t start, std::size_t len)
    {
        lock::spinlock_guard g(pmm_alloc_lock);
        auto index = meta_allocator.allocate();
        if(index == -1ull)
            klog::panic("pmm buffer exhausted");
        region[index] = pmm_region(start, len);
    }

    void* pmm_allocate(std::size_t len)
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard g(pmm_alloc_lock);
            if (region[i].exists())
            {
                auto ptr = region[i].allocate(len);
                if (ptr != 0)
                    return std::memset((void*)ptr, 0, paging::PAGE_SMALL_SIZE * len);
            }
        }

        return nullptr;
    }
} // namespace mm
