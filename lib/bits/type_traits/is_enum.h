#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_ENUM_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_ENUM_H__
#include "utils.h"

namespace std
{
#if __has_builtin(__is_enum)
    __nostd_type_traits_intrinsic_trait(enum)
#else
    static_assert(false, "required builtin does not exist for std::is_enum<T>");
#endif
}

#endif
