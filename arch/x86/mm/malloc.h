#pragma once

#include <cstddef>

namespace alloc
{
    void init(void* ptr, std::size_t s);
    void* malloc(std::size_t size);
    void* aligned_malloc(std::size_t s, std::size_t align);
    void* realloc(void* buf, std::size_t size);
    void free(void* buffer);
}
