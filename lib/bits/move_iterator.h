#ifndef __NOSTDLIB_MOVE_ITERATOR_H__
#define __NOSTDLIB_MOVE_ITERATOR_H__
#include "iterator_simple_types.h"
#include "../include/utility"

namespace std
{
    template <typename I>
    class move_iterator
    {
        I iter;
        template <typename It>
        friend class move_iterator;

    public:
        using iterator_type = I;
        using iterator_category = typename iterator_traits<I>::iterator_category;
        using value_type = typename iterator_traits<I>::value_type;
        using difference_type = typename iterator_traits<I>::difference_type;
        using pointer = I;
        using reference = conditional_t<is_reference_v<typename iterator_traits<I>::reference>,
                                        remove_reference_t<typename iterator_traits<I>::reference>&&,
                                        typename iterator_traits<I>::reference>;

        constexpr move_iterator() : iter() {}
        explicit constexpr move_iterator(iterator_type i) : iter(move(i)) {}

        template <typename It>
        constexpr move_iterator(const move_iterator<It>& i) : iter(i.iter)
        {
        }

        template <typename It>
        constexpr move_iterator& operator=(const move_iterator<It>& i)
        {
            iter = i.iter;
            return *this;
        }

        [[nodiscard]] constexpr const iterator_type& base() const& noexcept { return iter; }
        [[nodiscard]] constexpr iterator_type base() && { return std::move(iter); }
        [[nodiscard]] constexpr reference operator*() const { return static_cast<reference>(*iter); }
        [[nodiscard]] constexpr pointer operator->() const { return iter; }

        constexpr move_iterator& operator++()
        {
            ++iter;
            return *this;
        }

        constexpr move_iterator operator++(int)
        {
            move_iterator __tmp = *this;
            ++iter;
            return __tmp;
        }

        constexpr move_iterator& operator--()
        {
            --iter;
            return *this;
        }

        constexpr move_iterator operator--(int)
        {
            move_iterator tmp = *this;
            --iter;
            return tmp;
        }

        [[nodiscard]] constexpr move_iterator operator+(difference_type n) const { return move_iterator(iter + n); }
        [[nodiscard]] constexpr move_iterator operator-(difference_type n) const { return move_iterator(iter - n); }

        constexpr move_iterator& operator+=(difference_type n)
        {
            iter += n;
            return *this;
        }

        constexpr move_iterator& operator-=(difference_type n)
        {
            iter -= n;
            return *this;
        }

        template <typename ItL, typename ItR>
        [[nodiscard]] inline constexpr difference_type operator-(const move_iterator<ItR>& rhs)
        {
            return base() - rhs.base();
        }

        [[nodiscard]] constexpr reference operator[](difference_type n) const { return std::move(iter[n]); }
    };

    template <typename It>
    [[nodiscard]] constexpr move_iterator<It> make_move_iterator(It i)
    {
        return move_iterator<It>(std::move(i));
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator==(const move_iterator<It>& lhs, const move_iterator<It>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator!=(const move_iterator<It>& lhs, const move_iterator<It>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator<(const move_iterator<It>& lhs, const move_iterator<It>& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator<=(const move_iterator<It>& lhs, const move_iterator<It>& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator>(const move_iterator<It>& lhs, const move_iterator<It>& rhs)
    {
        return rhs < lhs;
    }

    template <typename It>
    [[nodiscard]] inline constexpr bool operator>=(const move_iterator<It>& lhs, const move_iterator<It>& rhs)
    {
        return !(lhs < rhs);
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator==(const move_iterator<ItL>& lhs, const move_iterator<ItR>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator!=(const move_iterator<ItL>& lhs, const move_iterator<ItR>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator<(const move_iterator<ItL>& lhs, const move_iterator<ItR>& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator<=(const move_iterator<ItL>& lhs, const move_iterator<ItR>& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator>(const move_iterator<ItL>& lhs, const move_iterator<ItR>& rhs)
    {
        return rhs < lhs;
    }

    template <typename ItL, typename ItR>
    [[nodiscard]] inline constexpr bool operator>=(const move_iterator<ItL>& lhs, const move_iterator<ItR>& rhs)
    {
        return !(lhs < rhs);
    }

    template <typename It>
    [[nodiscard]] inline constexpr move_iterator<It> operator+(typename move_iterator<It>::difference_type n,
                                                               const move_iterator<It>& lhs)
    {
        return lhs + n;
    }
} // namespace std

#endif
