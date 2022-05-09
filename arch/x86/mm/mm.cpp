#include "mm.h"

namespace mm
{
    bitmask_allocator::bitmask_allocator(void* buf, std::size_t len) : buf((uint64_t*)buf), len(len)
    {
        std::size_t n = len / 64;

        std::memset(buf, 0xff, n * sizeof(uint64_t));
        if(len % 64)
            this->buf[len / 64] = ~((1ul << (64 - len % 64)) - 1);
    }

    std::size_t bitmask_allocator::allocate()
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

    bool bitmask_allocator::allocate(std::size_t index)
    {
        bool ret = test(index);
        std::set_bit(buf[index / 64], false, 63 - index % 64);
        return ret;
    }

    void bitmask_allocator::free(std::size_t index)
    {
        std::set_bit(buf[index / 64], true, 63 - index % 64);
    }
} // namespace mm
