#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_COMPOUND_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_COMPOUND_H__
#include "is_arithmetic.h"
#include "is_enum.h"
#include "is_member_pointer.h"
#include "is_nullptr.h"
#include "pointer.h"
#include "utils.h"

namespace std
{
#if __has_builtin(__is_compound) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(compound);
#else
    template <typename T>
    struct is_compound : std::bool_constant<!std::is_fundamental_v<T>>
    {
    };
#endif
} // namespace std

#endif
