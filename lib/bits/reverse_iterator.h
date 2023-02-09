#ifndef __NOSTDLIB_BITS_REVERSE_ITERATOR_H__
#define __NOSTDLIB_BITS_REVERSE_ITERATOR_H__
#include "iterator_simple_types.h"

namespace std
{
    template <typename I>
    class reverse_iterator
        : public iterator<typename iterator_traits<I>::iterator_category, typename iterator_traits<I>::value_type,
                          typename iterator_traits<I>::difference_type, typename iterator_traits<I>::pointer,
                          typename iterator_traits<I>::reference>
    {
    protected:
        I iter;

    public:
        using iterator_type = I;
        typedef typename iterator_traits<I>::pointer pointer;
        typedef typename iterator_traits<I>::difference_type difference_type;
        typedef typename iterator_traits<I>::reference reference;

    private:
        template <typename It>
        friend class reverse_iterator;

        template <typename T>
        static constexpr T* convert_iter_to_ptr(T* p)
        {
            return p;
        }
        template <typename T>
        static constexpr pointer convert_iter_to_ptr(T t)
        {
            return t.operator->();
        }

    public:
        constexpr reverse_iterator() noexcept(noexcept(I())) : iter() {}
        explicit constexpr reverse_iterator(iterator_type t) noexcept(noexcept(I(t))) : iter(t) {}
        constexpr reverse_iterator(const reverse_iterator& i) noexcept(noexcept(I(i.current))) : iter(i.current) {}
        reverse_iterator& operator=(const reverse_iterator&) = default;
        template <typename It>
        constexpr reverse_iterator(const reverse_iterator<It>& i) noexcept(noexcept(I(i.iter))) : iter(i.iter)
        {
        }

        [[nodiscard]] constexpr iterator_type base() const noexcept(noexcept(I(iter))) { return iter; }

        template <typename It>
        constexpr reverse_iterator& operator=(const reverse_iterator<It>& i) noexcept(noexcept(iter = i.iter))
        {
            iter = i.iter;
            return *this;
        }

        [[nodiscard]] constexpr reference operator*() const
        {
            I tmp = iter;
            return *--tmp;
        }

        [[nodiscard]] constexpr pointer operator->() const
        {
            I tmp = iter;
            --tmp;
            return convert_iter_to_ptr(tmp);
        }

        constexpr reverse_iterator& operator++()
        {
            --iter;
            return *this;
        }

        constexpr reverse_iterator operator++(int)
        {
            reverse_iterator __tmp = *this;
            --iter;
            return __tmp;
        }

        constexpr reverse_iterator& operator--()
        {
            ++iter;
            return *this;
        }

        constexpr reverse_iterator operator--(int)
        {
            reverse_iterator tmp = *this;
            ++iter;
            return tmp;
        }

        [[nodiscard]] constexpr reverse_iterator operator+(difference_type n) const { return reverse_iterator(iter - n); }
        [[nodiscard]] constexpr reverse_iterator operator-(difference_type n) const { return reverse_iterator(iter + n); }

        constexpr reverse_iterator& operator+=(difference_type n)
        {
            iter -= n;
            return *this;
        }

        constexpr reverse_iterator& operator-=(difference_type n)
        {
            iter += n;
            return *this;
        }

        template <typename ItL, typename ItR>
        [[nodiscard]] inline constexpr difference_type operator-(const reverse_iterator<ItR>& rhs)
        {
            return base() - rhs.base();
        }

        [[nodiscard]] constexpr reference operator[](difference_type __n) const { return *(*this + __n); }
    };

    template <typename It>
    [[nodiscard]] constexpr reverse_iterator<It> make_reverse_iterator(It i)
    {
        return reverse_iterator<It>(i);
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator==(const reverse_iterator<It>& lhs, const reverse_iterator<It>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator!=(const reverse_iterator<It>& lhs, const reverse_iterator<It>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator<(const reverse_iterator<It>& lhs, const reverse_iterator<It>& rhs)
    {
        return rhs.base() < lhs.base();
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator>(const reverse_iterator<It>& lhs, const reverse_iterator<It>& rhs)
    {
        return rhs < lhs;
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator<=(const reverse_iterator<It>& lhs, const reverse_iterator<It>& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator>=(const reverse_iterator<It>& lhs, const reverse_iterator<It>& rhs)
    {
        return !(lhs < rhs);
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator==(const reverse_iterator<ItL>& lhs, const reverse_iterator<ItR>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator!=(const reverse_iterator<ItL>& lhs, const reverse_iterator<ItR>& rhs)
    {
        return lhs.base() != rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator<(const reverse_iterator<ItL>& lhs, const reverse_iterator<ItR>& rhs)
    {
        return lhs.base() > rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator>(const reverse_iterator<ItL>& lhs, const reverse_iterator<ItR>& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator<=(const reverse_iterator<ItL>& lhs, const reverse_iterator<ItR>& rhs)
    {
        return lhs.base() >= rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator>=(const reverse_iterator<ItL>& lhs, const reverse_iterator<ItR>& rhs)
    {
        return lhs.base() <= rhs.base();
    }

    template <typename It>
    [[nodiscard]] inline constexpr reverse_iterator<It> operator+(typename reverse_iterator<It>::difference_type n,
                                                                  const reverse_iterator<It>& lhs)
    {
        return lhs + n;
    }
} // namespace std

#endif
