#ifndef __ARCH_X86_ID_ALLOCATOR_H__
#define __ARCH_X86_ID_ALLOCATOR_H__
#include <bitmanip.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

template <std::size_t len, bool inplace = len < 512>
class id_allocator
{
    using storage = std::conditional_t<inplace, uint64_t[std::detail::div_roundup(len, 64ul) + 1], uint64_t*>;

    storage buf;

public:
    id_allocator()
    {
        if constexpr (!inplace)
            buf = new uint64_t[std::detail::div_roundup(len, 64ul)];

        std::size_t n = len;

        std::memset(buf, 0xff, n / 8);
        if (len % 64)
            this->buf[n / 64 + 1] = 1 << ((n % 64) - 1);
    }

    ~id_allocator()
    {
        if constexpr (!inplace)
            delete buf;
    }

    id_allocator(const id_allocator&) = delete;
    id_allocator& operator=(const id_allocator& rhs) = delete;

    std::size_t allocate()
    {
        if (!exists())
            return -1ul;

        std::size_t metadata_size = std::detail::div_roundup(len, 64ul);
        for (std::size_t i = 0; i < metadata_size; i++)
        {
            if (buf[i])
            {
                std::size_t bitindex = __builtin_clzll(buf[i]);
                std::set_bit(buf[i], false, 63 - bitindex);

                std::size_t offset = (64 * i + bitindex);
                return offset;
            }
        }
        return -1ul;
    }

    bool allocate(std::size_t index)
    {
        bool ret = test(index);
        std::set_bit(buf[index / 64], false, 63 - index % 64);
        return ret;
    }

    void free(std::size_t index) { std::set_bit(buf[index / 64], true, 63 - index % 64); }
    constexpr std::size_t size() { return len; }
    constexpr bool exists() { return len != 0; }
    constexpr void* get_buffer() { return (void*)buf; }
    constexpr bool test(std::size_t i) { return std::get_bit(buf[i / 64], 63 - i % 64); }
};

#endif
