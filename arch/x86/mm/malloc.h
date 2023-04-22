#pragma once

#include <cstddef>

namespace alloc
{
    void init(void* ptr, std::size_t size);
    auto malloc(std::size_t size) -> void*;
    auto aligned_malloc(std::size_t size, std::size_t align) -> void*;
    auto realloc(void* buf, std::size_t size) -> void*;
    void free(void* buffer);
} // namespace alloc
