#include "mm.h"

namespace mm
{
    bitmask_allocator::bitmask_allocator(void* buf, std::size_t len) : buf((uint64_t*)buf), len(len)
    {
        // TODO: cleanp detail
        if (buf == nullptr)
        {
            buf = nullptr;
            len = 0;
            return;
        }

        std::memset(buf, 0xff, len / 8);
        if (len % 64)
            this->buf[len / 64 + 1] = 1 << ((len % 64) - 1);
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

                std::size_t offset = (64 * i + 63 - bitindex);
                return offset;
            }
        }
        return -1ul;
    }
} // namespace mm
