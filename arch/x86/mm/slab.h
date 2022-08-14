#ifndef __ARCH_X86_MM_SLAB_H__
#define __ARCH_X86_MM_SLAB_H__
#include <cstddef>

namespace mm
{
    void init_slab();

    void* slab_allocate(std::size_t n);
    void slab_free(void* ptr);

    template<typename T>
    T* slab_allocate()
    {
        return new (slab_allocate(sizeof(T))) T;
    }
}

#endif