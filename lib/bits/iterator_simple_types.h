#ifndef __NOSTDLIB_BITS_ITERATOR_SIMPLE_TYPES_H__
#define __NOSTDLIB_BITS_ITERATOR_SIMPLE_TYPES_H__
#include "../include/type_traits"
#include "../include/concepts"

namespace std
{
    struct input_iterator_tag
    {
    };

    struct output_iterator_tag
    {
    };

    struct forward_iterator_tag : public input_iterator_tag
    {
    };

    struct bidirectional_iterator_tag : public forward_iterator_tag
    {
    };

    struct random_access_iterator_tag : public bidirectional_iterator_tag
    {
    };

    struct contiguous_iterator_tag : public random_access_iterator_tag
    {
    };

    template <typename C, typename T, typename D = ptrdiff_t, typename P = T*, typename R = T&>
    struct iterator
    {
        using iterator_category = C;
        using value_type = T;
        using difference_type = D;
        using pointer = P;
        using reference = R;
    };

    template <typename Iter>
    struct iterator_traits;

    namespace detail
    {
        template <typename I, typename = void_t<>>
        struct iterator_traits_helper
        {
        };

        template <typename I>
        struct iterator_traits_helper<I, void_t<typename I::iterator_category, typename I::value_type,
                                                typename I::difference_type, typename I::pointer, typename I::reference>>
        {
            using iterator_category = typename I::iterator_category;
            using value_type = typename I::value_type;
            using difference_type = typename I::difference_type;
            using pointer = typename I::pointer;
            using reference = typename I::reference;
        };
    } // namespace detail

    template <typename I>
    struct iterator_traits : public detail::iterator_traits_helper<I>
    {
    };

    template <typename T>
    struct iterator_traits<T*>
    {
        using iterator_concept = contiguous_iterator_tag;
        using iterator_category = random_access_iterator_tag;
        using value_type = remove_cv_t<T>;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    };

    namespace detail
    {
        template <typename I>
        using iterator_category_t = typename iterator_traits<I>::iterator_category;

        template <typename I>
        using require_input_ierator = enable_if_t<is_convertible<iterator_category_t<I>, input_iterator_tag>::value>;

        template <typename I, typename C = iterator_category_t<I>>
        using is_random_access_iter = is_base_of<random_access_iterator_tag, C>;
    } // namespace detail
} // namespace std

#endif
