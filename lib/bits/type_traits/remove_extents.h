#ifndef __NOSTDLIB_TYPE_TRAITS_REMOVE_EXTENTS_H__
#define __NOSTDLIB_TYPE_TRAITS_REMOVE_EXTENTS_H__
#include "../../include/cstddef"

namespace std
{
    template <typename T>
    struct remove_extent
    {
        using type = T;
    };

    template <typename T>
    struct remove_extent<T[]>
    {
        using type = T;
    };

    template <typename T, std::size_t N>
    struct remove_extent<T[N]>
    {
        using type = T;
    };

    template <typename T>
    using remove_extent_t = typename remove_extent<T>::type;

    template <typename T>
    struct remove_all_extents
    {
        using type = T;
    };

    template <typename T>
    struct remove_all_extents<T[]>
    {
        using type = typename remove_all_extents<T>::type ;
    };

    template <typename T, std::size_t N>
    struct remove_all_extents<T[N]>
    {
        using type = typename remove_all_extents<T>::type ;
    };

    template <typename T>
    using remove_all_extents_t = typename remove_all_extents<T>::type;
} // namespace std

#endif
