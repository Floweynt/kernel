#include "mm.h"
#include "bitmanip.h"
#include <bitset>
#include <debug/debug.h>

namespace mm
{ 
    std::size_t bitmask_allocator::allocate(std::size_t s)
    {
        if (!exists())
            return -1ul;
        if(s != 1)
            debug::panic("not implemented yet");
        std::size_t i = std::first_set_bit(bitmask);
        if(i == -1ul)
            return -1ul;
        bitmask.reset(i);
        return i;
    }
} // namespace mm
