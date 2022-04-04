#ifndef __ARCH_X86_PMM_PMM_H__
#define __ARCH_X86_PMM_PMM_H__
#include <cstddef.h>
#include <utils.h>

namespace pmm
{
    class pmm_region
    {
        void* region_start;
        std::size_t len;
        void* meta;

    public:
        pmm_region(void* region_start, std::size_t len) : region_start(region_start), len(len)
        {
            std::size_t meta_layer = (ceil_logbase2(len) + 5) / 6;
        }
    };
} // namespace pmm

#endif
