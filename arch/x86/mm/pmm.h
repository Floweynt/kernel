#ifndef __ARCH_X86_MM_PMM_H__
#define __ARCH_X86_MM_PMM_H__
#include "mm.h"
#include "paging/paging.h"
namespace mm
{
    class pmm_region : public bitmask_allocator
    {
    public:
        pmm_region() : bitmask_allocator() {}
        pmm_region(void* buf, std::size_t len) : bitmask_allocator(buf, len - metadata_size_pages(len)) {}

        inline void* allocate(std::size_t len)
        {
            std::size_t val = bitmask_allocator::allocate(len);
            if (val == -1ul)
                return nullptr;

            return (void*)(((uint8_t*)get_buffer()) + ((val + metadata_size_pages(size())) * paging::PAGE_SMALL_SIZE));
        }

        inline void free(void* index)
        {
            auto diff = ((uint8_t*)get_buffer() - (uint8_t*)index);
            std::size_t page = diff / paging::PAGE_SMALL_SIZE;
            page -= metadata_size_pages(size());
            bitmask_allocator::free(page);
        }
    };

    void add_region(void*, std::size_t);
    void add_region_pre_smp(void*, std::size_t);
    void* pmm_allocate(std::size_t len = 1);
    void* pmm_allocate_pre_smp(std::size_t len = 1);
} // namespace mm

#endif
