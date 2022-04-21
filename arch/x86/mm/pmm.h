#ifndef __ARCH_X86_MM_PMM_H__
#define __ARCH_X86_MM_PMM_H__
#include "mm.h"
namespace mm
{
    class pmm_region : public bitmask_allocator
    {
    public:
        pmm_region() : bitmask_allocator() {}
        pmm_region(void* buf, std::size_t len) : bitmask_allocator(buf, len) {}

        inline void* allocate()
        {
            return (void*)(((uint8_t*)get_buffer()) +
                           ((bitmask_allocator::allocate() + bitmask_allocator::metadata_size_pages(size())) * 4096));
        }
    };

    void add_region(void*, std::size_t);
    void add_region_pre_smp(void*, std::size_t);
    void* pmm_allocate();
    void* pmm_allocate_pre_smp();
} // namespace mm

#endif
