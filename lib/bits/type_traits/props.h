#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_PROPS_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_PROPS_H__
#include "utils.h"
#include "../../include/cstddef"

namespace std
{
    template <typename T>
    struct alignment_of : integral_constant<size_t, alignof(T)>
    {
    };

    template <typename T>
    inline constexpr size_t alignment_of_v = alignof(T);

#if __has_builtin(__array_rank) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T>
    struct rank : integral_constant<size_t, __array_rank(T)>
    {
    };

    template <typename T>
    inline constexpr size_t rank_v = __array_rank(T);
#else
    template <typename T>
    struct rank : public integral_constant<size_t, 0>
    {
    };

    template <typename T>
    struct rank<T[]> : public integral_constant<size_t, rank<T>::value + 1>
    {
    };

    template <typename T, size_t N>
    struct rank<T[N]> : public integral_constant<size_t, rank<T>::value + 1>
    {
    };

    template <typename T>
    inline constexpr size_t rank_v = rank<T>::value;
#endif

#if __has_builtin(__array_extent) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T, size_t N = 0>
    struct extent : integral_constant<size_t, __array_extent(T, N)>
    {
    };

    template <typename T, size_t N = 0>
    inline constexpr size_t extent_v = __array_extent(T, N);

#else
    template <class T, unsigned N = 0>
    struct extent : integral_constant<size_t, 0>
    {
    };

    template <class T>
    struct extent<T[], 0> : integral_constant<size_t, 0>
    {
    };

    template <class T, unsigned N>
    struct extent<T[], N> : extent<T, N - 1>
    {
    };

    template <class T, size_t I>
    struct extent<T[I], 0> : integral_constant<size_t, I>
    {
    };

    template <class T, size_t I, unsigned N>
    struct extent<T[I], N> : extent<T, N - 1>
    {
    };
#endif
} // namespace std

#endif
