#ifndef BITS_ITERATOR_UTILS_H
#define BITS_ITERATOR_UTILS_H
#include "iterator_simple_types.h"
#include "../include/type_traits"

namespace std
{
    namespace detail
    {
        template <typename I, typename C>
        class iterator_wrapper
        {
        protected:
            I current;

            template <typename It>
            using ignored = std::enable_if_t<std::is_convertible<It, I>::value>;

        public:
            using iterator_type = I;
            using iterator_category = typename std::iterator_traits<I>::iterator_category;
            using value_type = typename std::iterator_traits<I>::value_type;
            using difference_type = typename std::iterator_traits<I>::difference_type;
            using reference = typename std::iterator_traits<I>::reference;
            using pointer = typename std::iterator_traits<I>::pointer;

            constexpr iterator_wrapper() noexcept : current(I()) {}
            explicit iterator_wrapper(const I& i) noexcept : current(i) {}
            template <typename It, typename = std::enable_if_t<std::is_convertible_v<It, I>>>
            iterator_wrapper(const iterator_wrapper<It, C>& i) noexcept : current(i.base())
            {
            }

            reference operator*() const noexcept { return *current; }
            pointer operator->() const noexcept { return current; }
            reference operator[](difference_type n) const noexcept { return current[n]; }

            iterator_wrapper& operator++() noexcept
            {
                ++current;
                return *this;
            }

            iterator_wrapper operator++(int) noexcept { return iterator_wrapper(current++); }

            iterator_wrapper& operator--() noexcept
            {
                --current;
                return *this;
            }

            iterator_wrapper operator--(int) noexcept { return iterator_wrapper(current--); }

            iterator_wrapper& operator+=(difference_type n) noexcept
            {
                current += n;
                return *this;
            }

            iterator_wrapper& operator-=(difference_type n) noexcept
            {
                current -= n;
                return *this;
            }

            iterator_wrapper operator+(difference_type n) const noexcept { return iterator_wrapper(current + n); }
            iterator_wrapper operator-(difference_type n) const noexcept { return iterator_wrapper(current - n); }

            const I& base() const noexcept { return current; }
        };

        template <typename It, typename C>
        [[nodiscard]] constexpr inline bool operator==(const iterator_wrapper<It, C>& lhs,
                                                       const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() == rhs.base();
        }
        template <typename It, typename C>

        [[nodiscard]] constexpr inline bool operator!=(const iterator_wrapper<It, C>& lhs,
                                                       const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() != rhs.base();
        }

        template <typename It, typename C>
        [[nodiscard]] constexpr inline bool operator<(const iterator_wrapper<It, C>& lhs,
                                                      const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() < rhs.base();
        }

        template <typename It, typename C>
        [[nodiscard]] constexpr inline bool operator>(const iterator_wrapper<It, C>& lhs,
                                                      const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() > rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] constexpr inline bool operator==(const iterator_wrapper<ItL, C>& lhs,
                                                       const iterator_wrapper<ItR, C>& rhs) noexcept
        {
            return lhs.base() == rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] constexpr inline bool operator!=(const iterator_wrapper<ItL, C>& lhs,
                                                       const iterator_wrapper<ItR, C>& rhs) noexcept
        {
            return lhs.base() != rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] inline bool operator<(const iterator_wrapper<ItL, C>& lhs,
                                            const iterator_wrapper<ItR, C>& rhs) noexcept
        {
            return lhs.base() < rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] inline bool operator>(const iterator_wrapper<ItL, C>& lhs,
                                            const iterator_wrapper<ItR, C>& rhs) noexcept
        {
            return lhs.base() > rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] inline bool operator<=(const iterator_wrapper<ItL, C>& lhs,
                                             const iterator_wrapper<ItR, C>& rhs) noexcept
        {
            return lhs.base() <= rhs.base();
        }

        template <typename It, typename C>
        [[nodiscard]] constexpr inline bool operator<=(const iterator_wrapper<It, C>& lhs,
                                                       const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() <= rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] inline bool operator>=(const iterator_wrapper<ItL, C>& lhs,
                                             const iterator_wrapper<ItR, C>& rhs) noexcept
        {
            return lhs.base() >= rhs.base();
        }

        template <typename It, typename C>
        [[nodiscard]] constexpr inline bool operator>=(const iterator_wrapper<It, C>& lhs,
                                                       const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() >= rhs.base();
        }

        template <typename ItL, typename ItR, typename C>
        [[nodiscard]] constexpr inline auto operator-(const iterator_wrapper<ItL, C>& lhs,
                                                      const iterator_wrapper<ItR, C>& rhs) noexcept
            -> decltype(lhs.base() - rhs.base())

        {
            return lhs.base() - rhs.base();
        }

        template <typename It, typename C>
        [[nodiscard]] constexpr inline typename iterator_wrapper<It, C>::difference_type operator-(
            const iterator_wrapper<It, C>& lhs, const iterator_wrapper<It, C>& rhs) noexcept
        {
            return lhs.base() - rhs.base();
        }

        template <typename It, typename C>
        [[nodiscard]] constexpr inline iterator_wrapper<It, C> operator+(typename iterator_wrapper<It, C>::difference_type n,
                                                                         const iterator_wrapper<It, C>& i) noexcept
        {
            return iterator_wrapper<It, C>(i.base() + n);
        }
    } // namespace detail
} // namespace std

#endif
