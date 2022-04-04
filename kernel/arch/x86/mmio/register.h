#include <cstddef.h>
#include <cstdint.h>

template <typename T, std::size_t alignment>
class alignas(alignment) register_rw
{
    volatile T t;

public:
    T read() { return t; }
    void write(T v) { t = v; }
    constexpr operator T() const { return t; }
};

template <typename T, std::size_t alignment>
class alignas(alignment) register_rdonly
{
    volatile T t;

public:
    T read() { return t; }
    constexpr operator T() const { return t; }
};

template <typename T, std::size_t alignment>
class alignas(alignment) register_wronly
{
    volatile T t;

public:
    void write(T v) { t = v; }
};

template <typename T, std::size_t alignment>
struct alignas(alignment) register_reserved
{
};
