#pragma once

#include <bitset>
#include <cstring>
#include <type_traits>

template <std::size_t len>
class id_allocator
{
    std::bitset<len> b;

public:
    constexpr id_allocator() { b.set(); }
    ~id_allocator() = default;
    id_allocator(const id_allocator&) = delete;
    auto operator=(const id_allocator& rhs) -> id_allocator& = delete;

    auto allocate() -> std::size_t
    {
        if (!exists())
        {
            return -1UL;
        }
        auto bit = std::first_set_bit(b);
        if (bit == -1UL)
        {
            return -1UL;
        }
        b.reset(bit);
        return bit;
    }

    auto allocate(std::size_t index) -> bool
    {
        bool ret = test(index);
        b.reset(index);
        return ret;
    }

    void free(std::size_t index) { b.set(index); }
    constexpr auto size() -> std::size_t { return len; }
    constexpr auto exists() -> bool { return len != 0; }
    constexpr auto test(std::size_t index) -> bool { return b.test(index); }
};

class dynamic_id_allocator
{
    std::dynamic_bitset bits;

public:
    constexpr dynamic_id_allocator(std::size_t len) : bits(len)
    {
        std::size_t meta = 1UL << std::ceil_logbase2(len);
        std::size_t idx = 0;
        for (std::size_t i = 0; i <= std::ceil_logbase2(len); i++)
        {
            std::size_t bits_to_set = std::div_roundup(len, meta);
            meta >>= 1;
            for (std::size_t j = 0; j < 1ul << i; j++)
            {
                if (bits_to_set)
                {
                    bits.set(idx++, true);
                    bits_to_set--;
                }
                else
                {
                    bits.set(idx++, false);
                }
            }
        }
    }

    constexpr dynamic_id_allocator(const dynamic_id_allocator& rhs) = default;

    constexpr dynamic_id_allocator() : bits(1) {}

    constexpr auto allocate() -> std::size_t
    {
        if (!bits.test(0))
        {
            return -1UL;
        }
        std::size_t index = 0;
        while (true)
        {
            if (2 * index + 1 >= bits.size())
            {
                break;
            }
            if (bits.test(2 * index + 1))
            {
                index = 2 * index + 1;
            }
            else
            {
                index = 2 * index + 2;
            }
        }

        bits.reset(index);
        std::size_t ret = index - ((1UL << std::ceil_logbase2(bits.size())) - 1);

        while (index != 0)
        {
            bits.set((index - 1) / 2, bits.test(index) || bits.test(index % 2 ? index + 1 : index - 1));
            index = (index - 1) / 2;
        }

        bits.set(0, bits.test(1) || bits.test(2));

        return ret;
    }

    constexpr void free(std::size_t idx)
    {
        std::size_t index = idx + ((1UL << std::ceil_logbase2(bits.size())) - 1);
        while (index != 0)
        {
            bits.set(index, true);
            index = (index - 1) / 2;
        }

        bits.set(0, true);
    }
};
