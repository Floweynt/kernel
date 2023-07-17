#include <mm/mm.h>
#include "bitmanip.h"
#include <bitset>
#include <debug/debug.h>

namespace mm
{
    auto bitmask_allocator::allocate(std::size_t size) -> std::size_t
    {
        if (!exists())
        {
            return -1UL;
        }
        if (size != 1)
        {
            debug::panic("not implemented yet");
        }
        std::size_t index = std::first_set_bit(bitmask);
        if (index == -1UL)
        {
            return -1UL;
        }
        bitmask.reset(index);
        return index;
    }
} // namespace mm
