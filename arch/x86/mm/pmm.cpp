#include "pmm.h"
#include <kinit/limine.h>
#include <config.h>
#include <cstdint>
#include <paging/paging.h>
#include <sync/spinlock.h>
#include <utils/id_allocator.h>

namespace mm
{
    inline constexpr auto PMM_COUNT = config::get_val<"pmm-count">;
    pmm_region region[PMM_COUNT];
    static id_allocator<PMM_COUNT> meta_allocator;
    static lock::spinlock l;

    void init()
    {
        boot_resource::instance().iterate_mmap([](const limine_memmap_entry& e) {
            if (e.type == 1)
                mm::add_region(mm::make_virtual(e.base), e.length / paging::PAGE_SMALL_SIZE);
        });
    }

    void add_region(std::uintptr_t start, std::size_t len)
    {
        lock::spinlock_guard g(l);
        region[meta_allocator.allocate()] = pmm_region(start, len);
    }

    void* pmm_allocate(std::size_t len)
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard g(l);
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
