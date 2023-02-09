#pragma once

#include "mm.h"
#include "paging/paging.h"

namespace mm
{
    class pmm_region : public bitmask_allocator
    {
    public:
        pmm_region() : bitmask_allocator() {}
        pmm_region(std::uintptr_t buf, std::size_t len) : bitmask_allocator((void*)buf, len - metadata_size_pages(len)) {}

        inline std::uintptr_t allocate(std::size_t len)
        {
            std::size_t val = bitmask_allocator::allocate(len);
            if (val == -1ul)
                return 0;
            return (std::uintptr_t)get_buffer() + (val + metadata_size_pages(size())) * paging::PAGE_SMALL_SIZE;
        }

        inline void free(std::uintptr_t index)
        {
            auto diff = ((std::uint8_t*)get_buffer() - (std::uint8_t*)index);
            std::size_t page = diff / paging::PAGE_SMALL_SIZE;
            page -= metadata_size_pages(size());
            bitmask_allocator::free(page);
        }
    };

    void init();

    void add_region(std::uintptr_t, std::size_t);
    void* pmm_allocate(std::size_t len = 1);
} // namespace pmm
