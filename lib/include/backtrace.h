#pragma once
#include "cstddef"

namespace std
{
    auto backtrace(size_t skip, size_t count, void** buf, size_t* base_ptr = nullptr) -> size_t;
}
