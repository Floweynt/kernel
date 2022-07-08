#include "block_alloc.h"
#include <../bits/utils.h>

std::size_t block_allocate(uint64_t* meta_buf, std::size_t buf_size, std::size_t len)
{
    // rather trivial version
    if (len == 1)
    {
        auto metadata_size = std::detail::div_roundup(buf_size, 64ul);
        for (std::size_t i = 0; i < metadata_size; i++)
        {
            if (meta_buf[i])
            {
                std::size_t bitindex = __builtin_ctzll(meta_buf[i]);
                std::set_bit(meta_buf[i], false, bitindex);

                std::size_t offset = (64 * i + bitindex);
                return offset;
            }
        }
        return -1ul;
    }

    std::size_t n = 0;
    // build sliding window
    for (std::size_t i = 0; i < len / 64; i++)
        n += __builtin_popcountll(meta_buf[i]);
    // handle edge case
    if (len % 64)
        n += __builtin_popcountll(((1 << (len % 64)) - 1) & meta_buf[len / 64]);

    if (n == len)
        return 0;

    for (std::size_t i = 0; i + len < buf_size; i++)
    {
        n -= std::get_bit(meta_buf[i / 64], i % 64);
        n += std::get_bit(meta_buf[(i + len) / 64], (i + len) % 64);
        if (n == len)
            return i + 1;
    }
    return -1ul;
}

std::size_t block_allocate(uint64_t* meta_buf, std::size_t buf_size, std::size_t len, std::size_t& r)
{
    return block_allocate(meta_buf, buf_size, len);
}

void block_free(uint64_t* meta_buf, std::size_t len, std::size_t index) 
{
    for(std::size_t i = 0; i < len; i++)
        std::set_bit(meta_buf[(index + i) / 64], false, (index + i) % 64);  
}
