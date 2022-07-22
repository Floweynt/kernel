#include "pmm.h"
#include <config.h>
#include <sync/spinlock.h>
#include <utils/id_allocator.h>
#include <paging/paging.h>

namespace mm
{
    inline constexpr auto PMM_COUNT = config::get_val<"pmm-count">;
    pmm_region region[PMM_COUNT];
    static id_allocator<PMM_COUNT> meta_allocator;
    static lock::spinlock l;

    void add_region_pre_smp(void* start, std::size_t len)
    {
        region[meta_allocator.allocate()] = pmm_region(start, len);
    }

    void add_region(void* start, std::size_t len)
    {
        lock::spinlock_guard g(l);
        region[meta_allocator.allocate()] = pmm_region(start, len);
    }

    void* pmm_allocate_pre_smp(std::size_t len)
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard g(l, 0);
            if (region[i].exists())
            {
                auto ptr = region[i].allocate(len);
                if (ptr != nullptr)
                    return std::memset(ptr, 0, paging::PAGE_SMALL_SIZE * len);
            }
        }

        return nullptr;
    }

    void* pmm_allocate(std::size_t len)
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard g(l);
            if (region[i].exists())
            {
                auto ptr = region[i].allocate(len);
                if (ptr != nullptr)
                    return std::memset(ptr, 0, paging::PAGE_SMALL_SIZE * len);
            }
        }

        return nullptr;
    }
} // namespace mm
