#include "bitmanip.h"
#include "bits/mathhelper.h"
#include "bits/utils.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mm/mm.h>
#include <utility>
#include <utils/utils.h>

namespace mm
{
    class slab : public bitmask_allocator
    {
        std::size_t obj_size;
        void* buf;
    public:
        slab(void* buf, std::size_t len, std::size_t obj_size)
            : bitmask_allocator(new uint8_t[std::div_roundup(len, 8)], len), obj_size(obj_size), buf(buf)
        {
        }

        void* allocate(std::size_t len)
        {
            std::size_t val = bitmask_allocator::allocate(len);
            if (val == -1ul)
                return nullptr;

            return (void*)((uint8_t*)buf + (val * obj_size));
        }

        void free(void* index)
        {
            auto diff = ((uint8_t*)buf - (uint8_t*)index);
            std::size_t page = diff / obj_size;
            bitmask_allocator::free(page);
        }

        bool in_bounds(void* ptr)
        {
            auto p = (uint64_t) ptr;
            auto b = (uint64_t) buf;

            return p > b && p < (b + obj_size * size());
        }
    };

    void init_slab()
    {
        // slab  shall contain object caches of size 2^n to 2^m, 
        // in which n = config::get_val<"slab.min_order"> and m = config::get_val<"slab.max_order">
        // The total size of each object cache shall be 2^(config::get_val<"slab.slab_size_order">
        for(std::size_t order = config::get_val<"slab.min_order">; i <= config::get_val<"slab.max_order">; i++)
        {
        
        }
    }
} // namespace mm
