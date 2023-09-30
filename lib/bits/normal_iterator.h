#pragma once

#include <iterator>
namespace std::detail
{
    template <typename It, typename C>
    class normal_iterator
    {
    protected:
        It current;

        using traits_type = std::iterator_traits<It>;

    public:
        using iterator_type = It;
        using iterator_category = typename traits_type::iterator_category;
        using value_type = typename traits_type::value_type;
        using difference_type = typename traits_type::difference_type;
        using reference = typename traits_type::reference;
        using pointer = typename traits_type::pointer;

        constexpr normal_iterator() noexcept : current(It()) {}

        explicit constexpr normal_iterator(const It& iter) noexcept : current(iter) {}

        template <typename I>
            requires(std::is_convertible_v<I, It>)
        constexpr normal_iterator(const normal_iterator<I, C>& iter) noexcept

            : current(iter.base())
        {
        }

        // Forward iterator requirements
        constexpr reference operator*() const noexcept { return *current; }

        constexpr pointer operator->() const noexcept { return current; }

        constexpr normal_iterator& operator++() noexcept
        {
            ++current;
            return *this;
        }

        constexpr normal_iterator operator++(int) noexcept { return normal_iterator(current++); }

        // Bidirectional iterator requirements
        constexpr normal_iterator& operator--() noexcept
        {
            --current;
            return *this;
        }

        constexpr normal_iterator operator--(int) noexcept { return normal_iterator(current--); }

        // Random access iterator requirements
        constexpr reference operator[](difference_type n) const noexcept { return current[n]; }

        constexpr normal_iterator& operator+=(difference_type n) noexcept
        {
            current += n;
            return *this;
        }

        constexpr normal_iterator operator+(difference_type n) const noexcept { return normal_iterator(current + n); }

        constexpr normal_iterator& operator-=(difference_type n) noexcept
        {
            current -= n;
            return *this;
        }

        constexpr normal_iterator operator-(difference_type n) const noexcept { return normal_iterator(current - n); }

        constexpr const It& base() const noexcept { return current; }
    };

    template <typename ItL, typename ItR, typename C>
    [[nodiscard]] constexpr bool operator==(const normal_iterator<ItL, C>& lhs,
                                            const normal_iterator<ItR, C>& rhs) noexcept(noexcept(lhs.base() == rhs.base()))
        requires requires {
            {
                lhs.base() == rhs.base()
            } -> std::convertible_to<bool>;
        }
    {
        return lhs.base() == rhs.base();
    }

    template <typename ItL, typename ItR, typename C>
    [[nodiscard]] constexpr auto operator<=>(const normal_iterator<ItL, C>& lhs,
                                             const normal_iterator<ItR, C>& rhs) noexcept(noexcept(lhs.base() <=> rhs.base()))
    {
        return lhs.base() <=> rhs.base();
    }

    template <typename It, typename C>
    [[nodiscard]] constexpr bool operator==(const normal_iterator<It, C>& lhs,
                                            const normal_iterator<It, C>& rhs) noexcept(noexcept(lhs.base() == rhs.base()))
        requires requires {
            {
                lhs.base() == rhs.base()
            } -> std::convertible_to<bool>;
        }
    {
        return lhs.base() == rhs.base();
    }

    template <typename It, typename C>
    [[nodiscard]] constexpr auto operator<=>(const normal_iterator<It, C>& lhs,
                                             const normal_iterator<It, C>& rhs) noexcept(noexcept(lhs.base() <=> rhs.base()))
    {
        return lhs.base() <=> rhs.base();
    }

    template <typename ItL, typename ItR, typename C>
    [[nodiscard]] constexpr inline auto operator-(const normal_iterator<ItL, C>& lhs, const normal_iterator<ItR, C>& rhs) noexcept
        -> decltype(lhs.base() - rhs.base())
    {
        return lhs.base() - rhs.base();
    }

    template <typename It, typename C>
    [[nodiscard]] constexpr inline typename normal_iterator<It, C>::difference_type operator-(const normal_iterator<It, C>& lhs,
                                                                                              const normal_iterator<It, C>& rhs) noexcept
    {
        return lhs.base() - rhs.base();
    }

    template <typename It, typename C>
    [[nodiscard]] constexpr inline normal_iterator<It, C> operator+(typename normal_iterator<It, C>::difference_type n,
                                                                    const normal_iterator<It, C>& iter) noexcept
    {
        return normal_iterator<It, C>(iter.base() + n);
    }
} // namespace std::detail

