#ifndef __NOSTDLIB_BACKTRACE_H__
#define __NOSTDLIB_BACKTRACE_H__
#include "cstddef"

namespace std
{
    size_t backtrace(size_t skip, size_t count, void** buf, size_t* base_ptr = nullptr);
}

#endif
