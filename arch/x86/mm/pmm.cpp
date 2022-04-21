#include "pmm.h"
#include <config.h>
namespace mm
{
    pmm_region region[PMM_COUNT];
    static char buf[bitmask_allocator::metadata_size_pages(PMM_COUNT)];
    static bitmask_allocator meta_allocator((void*)buf, PMM_COUNT);
    static lock::spinlock l;

    void add_region_pre_smp(void* start, std::size_t len)
    {
        lock::spinlock_guard g(l, 0);
        region[meta_allocator.allocate()] = pmm_region(start, len);
    }

    void add_region(void* start, std::size_t len)
    {
        lock::spinlock_guard g(l);
        region[meta_allocator.allocate()] = pmm_region(start, len);
    }

    void* pmm_allocate_pre_smp()
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard g(l, 0);
            if (region[i].exists())
            {
                auto ptr = region[i].allocate();
                if (ptr != nullptr)
                    return std::memset(ptr, 0, 4096);
            }
        }

        return nullptr;
    }

    void* pmm_allocate()
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            lock::spinlock_guard g(l);
            if (region[i].exists())
            {
                auto ptr = region[i].allocate();
                if (ptr != nullptr)
                    return std::memset(ptr, 0, 4096);
            }
        }

        return nullptr;
    }
} // namespace mm
