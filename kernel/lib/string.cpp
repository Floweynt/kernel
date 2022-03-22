#include "string.h"
#include <arch/interface/properties.h>

void * memcpy ( void * destination, const void * source, size_t num )
{
    if constexpr(properties.test(HAS_MEMCPY))
        return archlib::memcpy(destination, source, num);
}
