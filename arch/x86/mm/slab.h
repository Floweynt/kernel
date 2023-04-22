#pragma once

#include <cstddef>

namespace mm
{
    void init_slab();

    auto slab_allocate(std::size_t size) -> void*;
    void slab_free(void* ptr);

    template<typename T>
    auto slab_allocate() -> T*
    {
        return new (slab_allocate(sizeof(T))) T;
    }
}
