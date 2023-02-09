#pragma once

#include <cstddef>
#include <cstdint>

namespace mmio
{
    template <typename T, std::size_t alignment>
    class alignas(alignment) register_rw
    {
        volatile T t;
    public:
        inline T read() { return t; }
        inline void write(T v) { t = v; }
        inline operator T() const { return t; }
    };

    template <typename T, std::size_t alignment>
    class alignas(alignment) register_rdonly
    {
        volatile T t;
    public:
        inline T read() { return t; }
        inline operator T() const { return t; }
    };

    template <typename T, std::size_t alignment>
    class alignas(alignment) register_wronly
    {
        volatile T t;
    public:
        inline void write(T v) { t = v; }
    };

    template <typename T, std::size_t alignment>
    class alignas(alignment) register_reserved
    {
        T t;
    };
} // namespace mmio
