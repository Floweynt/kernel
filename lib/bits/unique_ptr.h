#ifndef __NOSTDLIB_BITS_UNIQUE_PTR_H__
#define __NOSTDLIB_BITS_UNIQUE_PTR_H__
#include "../include/cstddef"
#include "../include/type_traits"
#include "../include/utility"

namespace std
{
    template <typename T>
    class unique_ptr
    {
    public:
        using pointer = T*;
        using element_type = T;

    private:
        pointer buf;

    public:
        constexpr unique_ptr() : buf(nullptr) {}
        constexpr unique_ptr(std::nullptr_t) {}
        constexpr explicit unique_ptr(pointer p) noexcept : buf(p) {}
        template <typename U>
        constexpr unique_ptr(unique_ptr<U>&& u) noexcept : buf(u.release())
        {
        }

        ~unique_ptr() noexcept { release(); }

        constexpr unique_ptr& operator=(unique_ptr&& r) noexcept
        {
            reset(r.buf);
            return *this;
        }
        constexpr unique_ptr& operator=(nullptr_t) noexcept { release(); };

        constexpr pointer release() noexcept
        {
            pointer tmp = buf;
            buf = nullptr;
            return buf;
        }

        constexpr pointer reset(pointer ptr = pointer()) noexcept
        {
            pointer tmp = buf;
            buf = ptr;
            if (tmp)
                delete tmp;
        }

        // constexpr void swap(unique_ptr& other) noexcept { std::swap(other.buf, buf); }
        constexpr pointer get() const noexcept { return buf; }
        constexpr explicit operator bool() const noexcept { return (bool)buf; }

        constexpr add_lvalue_reference_t<T> operator*() const noexcept(noexcept(*std::declval<pointer>())) { return *buf; }
        constexpr pointer operator->() const noexcept { return *buf; }
    };

    template <typename T>
    class unique_ptr<T[]>
    {
    public:
        using pointer = T*;
        using element_type = T;

    private:
        pointer buf;

    public:
        constexpr unique_ptr() : buf(nullptr) {}
        constexpr unique_ptr(std::nullptr_t) {}
        template <typename U>
        constexpr explicit unique_ptr(U p) noexcept : buf(p)
        {
        }
        template <typename U>
        constexpr unique_ptr(unique_ptr<U>&& u) noexcept : buf(u.release())
        {
        }

        ~unique_ptr() noexcept { release(); }

        constexpr unique_ptr& operator=(unique_ptr&& r) noexcept
        {
            reset(r.buf);
            return *this;
        }
        constexpr unique_ptr& operator=(nullptr_t) noexcept { release(); };

        constexpr pointer release() noexcept
        {
            pointer tmp = buf;
            buf = nullptr;
            return buf;
        }

        template <typename U>
        constexpr void reset(U ptr) noexcept
        {
            pointer tmp = buf;
            buf = ptr;
            if (tmp)
                delete[] tmp;
        }

        constexpr void reset(std::nullptr_t = nullptr) noexcept
        {
            pointer tmp = buf;
            buf = nullptr;
            if (tmp)
                delete[] tmp;
        }

        // constexpr void swap(unique_ptr& other) noexcept { std::swap(other.buf, buf); }
        constexpr pointer get() const noexcept { return buf; }
        constexpr explicit operator bool() const noexcept { return (bool)buf; }

        constexpr add_lvalue_reference_t<T> operator*() const noexcept(noexcept(*std::declval<pointer>())) { return *buf; }
        constexpr pointer operator->() const noexcept { return *buf; }
        constexpr add_lvalue_reference_t<T> operator[](size_t i) const { return buf[i]; }
    };

    namespace detail
    {
        template <typename T>
        struct make_unique
        {
            using single_object = unique_ptr<T>;
        };
        template <typename T>
        struct make_unique<T[]>
        {
            using array = unique_ptr<T[]>;
        };
        template <typename T, size_t N>
        struct make_unique<T[N]>
        {
            struct invalid_type
            {
            };
        };

        template <typename T>
        using unique_ptr_t = typename make_unique<T>::single_object;
        template <typename T>
        using unique_ptr_array_t = typename make_unique<T>::array;
        template <typename T>
        using invalid_make_unique_t = typename make_unique<T>::invalid_type;
    } // namespace detail

    template <typename T, typename... Args>
    constexpr detail::unique_ptr_t<T> make_unique(Args&&... args)
    {
        return unique_ptr<T>(new T(std::forward(args)...));
    }

    template <typename T>
    constexpr detail::unique_ptr_array_t<T> make_unique(size_t n)
    {
        return unique_ptr<T>(new remove_extent_t<T>[n]());
    }

    template <typename T, typename... Args>
    constexpr detail::invalid_make_unique_t<T> make_unique(Args&&...) = delete;
} // namespace std
#endif
