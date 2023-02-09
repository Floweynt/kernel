#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_COPY_CONSTRUCTIBLE_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_COPY_CONSTRUCTIBLE_H__
#include "constructible.h"
#include "utils.h"
#include "cv.h"
#include "ref.h"

namespace std
{
    template <typename T>
    struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
    template <typename T>
    struct is_trivially_copy_constructible : is_trivially_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
    template <typename T>
    struct is_nothrow_copy_constructible : is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

    template <typename T>
    inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
    template <typename T>
    inline constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;
    template <typename T>
    inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;
} // namespace std

#endif


