#include "bits/mathhelper.h"
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <debug/debug.h>
#include <mm/mm.h>
#include <utility>
#include <utils/utils.h>
#include <utils/id_allocator.h>

namespace mm
{
    class slab
    {
        dynamic_id_allocator alloc;
        void* buf;
        std::size_t size;
        std::size_t obj_size;
    public:
        constexpr slab(void* buf, std::size_t len, std::size_t obj_size)
            : alloc(len), buf(buf), size(len), obj_size(obj_size)
        {
        }

        constexpr slab(const slab& rhs) = default;

        constexpr slab() : alloc(0), buf(nullptr) {}

        void* allocate()
        {
            return (void*)((std::uintptr_t) buf + obj_size * alloc.allocate());
        }

        void free(void* index)
        {
            alloc.free(((std::uintptr_t)buf - (std::uintptr_t)index) / obj_size);
        }

        bool in_bounds(void* ptr)
        {
            auto p = (std::uintptr_t)ptr;
            auto b = (std::uintptr_t)buf;

            return p > b && p < (b + obj_size * size);
        }
    };

    inline constexpr auto MIN_ORDER = config::get_val<"slab.min_order">;
    inline constexpr auto MAX_ORDER = config::get_val<"slab.max_order">;
    inline constexpr auto SLAB_SIZE_ORDER = config::get_val<"slab.slab_size_order">;

    static slab BUILTIN_SLABS[MAX_ORDER - MIN_ORDER + 1];
    void init_slab()
    {
        // slab  shall contain object caches of size 2^n to 2^m,
        // in which n = config::get_val<"slab.min_order"> and m = config::get_val<"slab.max_order">
        // The total size of each object cache shall be 2^(config::get_val<"slab.slab_size_order">
        std::size_t idx = 0;
        for (std::size_t order = MIN_ORDER; order <= MAX_ORDER; order++)
        {
            BUILTIN_SLABS[idx] = slab((void*)(config::get_val<"mmap.start.slab"> + idx * (1 << SLAB_SIZE_ORDER)),
                                      1 << (SLAB_SIZE_ORDER - order), 1 << order);
        }
    }

    auto slab_allocate(std::size_t n) -> void*
    {
        std::size_t order = std::ceil_logbase2(n);
        if (order > MAX_ORDER)
        {
            debug::panic("slab: cannot find object cache of the proper size", false);
            return nullptr;
        }

        order = std::max(order, MIN_ORDER);

        return BUILTIN_SLABS[order - MIN_ORDER].allocate();
    }

    void slab_free(void* ptr)
    {
        std::uintptr_t p = (std::uintptr_t)ptr;
        auto slab = (p - config::get_val<"mmap.start.slab">) / (1 << SLAB_SIZE_ORDER);
        BUILTIN_SLABS[slab].free(ptr);
    }
} // namespace mm
