#ifndef __ARCH_X86_UTILS_ID_ALLOCATOR_H__
#define __ARCH_X86_UTILS_ID_ALLOCATOR_H__
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
    id_allocator& operator=(const id_allocator& rhs) = delete;

    std::size_t allocate()
    {
        if (!exists())
            return -1ul;
        auto bit = std::first_set_bit(b);
        if (bit == -1ul)
            return -1ul;
        b.reset(bit);
        return bit;
    }

    bool allocate(std::size_t index)
    {
        bool ret = test(index);
        b.reset(index);
        return ret;
    }

    void free(std::size_t index) { b.set(index); }
    constexpr std::size_t size() { return len; }
    constexpr bool exists() { return len != 0; }
    constexpr bool test(std::size_t i) { return b.test(i); }
};

class dynamic_id_allocator 
{
    std::dynamic_bitset s;
public:
    constexpr dynamic_id_allocator(std::size_t len) : s(len)
    {
        std::size_t f = 1ul << std::ceil_logbase2(len);
        std::size_t idx = 0;
        for (std::size_t i = 0; i <= std::ceil_logbase2(len); i++)
        {
            std::size_t bits_to_set = std::div_roundup(len, f);
            f >>= 1;
            for (std::size_t j = 0; j < 1ul << i; j++)
            {
                if (bits_to_set)
                {
                    s.set(idx++, true);
                    bits_to_set--;
                }
                else
                    s.set(idx++, false);
            }
        }
    }

    constexpr dynamic_id_allocator(const dynamic_id_allocator& rhs) = default;

    constexpr dynamic_id_allocator() : s(1) {}

    constexpr std::size_t allocate()
    {
        if (!s.test(0))
            return -1ul;
        std::size_t index = 0;
        while (1)
        {
            if (2 * index + 1 >= s.size())
                break;
            if (s.test(2 * index + 1))
                index = 2 * index + 1;
            else
                index = 2 * index + 2;
        }

        s.reset(index);
        std::size_t ret = index - ((1ul << std::ceil_logbase2(s.size())) - 1);

        while (index != 0)
        {
            s.set((index - 1) / 2, s.test(index) || s.test(index % 2 ? index + 1 : index -1));
            index = (index - 1) / 2;
        }

        s.set(0, s.test(1) || s.test(2));

        return ret;
    }

    constexpr void free(std::size_t i)
    {
        std::size_t index  = i + ((1ul << std::ceil_logbase2(s.size())) - 1);
        while(index != 0)
        {
            s.set(index, true);
            index = (index - 1) / 2;
        }

        s.set(0, true);
    }
};

#endif
