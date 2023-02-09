#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_UNION_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_UNION_H__
#include "utils.h"

namespace std
{
#if __has_builtin(__is_union) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(union);
#else
    static_assert(false, "required builtin does not exist for std::is_union<T>");
#endif
}

#endif
