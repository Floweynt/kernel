#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_COPY_ASSIGNABLE_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_COPY_ASSIGNABLE_H__ 
#include "assignable.h"
#include "utils.h"
#include "cv.h"
#include "ref.h"

namespace std
{
    template <typename T>
    struct is_copy_assignable : is_assignable<T, add_lvalue_reference_t<add_const_t<T>>> {};
    template <typename T>
    struct is_trivially_copy_assignable : is_trivially_assignable<T, add_lvalue_reference_t<add_const_t<T>>> {};
    template <typename T>
    struct is_nothrow_copy_assignable : is_nothrow_assignable<T, add_lvalue_reference_t<add_const_t<T>>> {};

    template <typename T>
    inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
    template <typename T>
    inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;
    template <typename T>
    inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;
} // namespace std

#endif


