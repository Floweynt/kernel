#include "pmm.h"
#include <config.h>
namespace mm
{
    static pmm_region region[PMM_COUNT];

    void add_region(void* start, std::size_t len)
    {
        static char buf[bitmask_allocator::metadata_size_pages(PMM_COUNT)];
        static bitmask_allocator meta_allocator((void*)buf, PMM_COUNT);
        region[meta_allocator.allocate()] = pmm_region(start, len);
    }

    void* pmm_allocate()
    {
        for (std::size_t i = 0; i < PMM_COUNT; i++)
        {
            if (region[i].exists())
            {
                auto ptr = region[i].allocate();
                if (ptr != nullptr)
                    return std::memset(ptr, 0, 4096);
;
            }
        }

        return nullptr;
    }
} // namespace mm
