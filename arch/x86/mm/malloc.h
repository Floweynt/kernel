#ifndef __ARCH_X86_MM_MALLOC_H__
#define __ARCH_X86_MM_MALLOC_H__
#include <cstddef>

namespace alloc
{
    void init(void* ptr, std::size_t s);
    void* malloc(std::size_t size);
    void* realloc(void* buf, std::size_t size);
    void free(void* buffer);
}

#endif
