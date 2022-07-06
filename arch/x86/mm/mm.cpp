#include "mm.h"
#include <utils/block_alloc.h>

namespace mm
{
    bitmask_allocator::bitmask_allocator(void* buf, std::size_t len) : buf((uint64_t*)buf), len(len)
    {
        std::size_t n = len / 64;

        std::memset(buf, 0xff, n * sizeof(uint64_t));
        if(len % 64)
            this->buf[len / 64] = (1ul << (len % 64)) - 1;
    }

    std::size_t bitmask_allocator::allocate(std::size_t s)
    {

        if (!exists())
            return -1ul;

        return block_allocate(buf, len, s);
    }

    void bitmask_allocator::free(std::size_t index)
    {
        std::set_bit(buf[index / 64], true, index % 64);
    }
} // namespace mm
