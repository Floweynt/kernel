#ifndef __NOSTDLIB_BACK_INSERT_IERATOR_H__
#define __NOSTDLIB_BACK_INSERT_IERATOR_H__
#include "iterator_simple_types.h"
#include "../include/memory"

namespace std
{
    template <typename C>
    class back_insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
        C* container;

    public:
        using container_type = C;
        using difference_type = ptrdiff_t;

        explicit back_insert_iterator(C& c) : container(&c) {}

        auto operator=(const typename C::value_type& value) -> back_insert_iterator&
        {
            container->push_back(value);
            return *this;
        }

        auto operator=(typename C::value_type&& value) -> back_insert_iterator&
        {
            container->push_back(std::move(value));
            return *this;
        }

        [[nodiscard]] auto operator*() -> back_insert_iterator& { return *this; }
        auto operator++() -> back_insert_iterator& { return *this; }
          auto operator++(int) -> back_insert_iterator { return *this; }
    };

    template <typename C>
    [[nodiscard]] inline auto back_inserter(C& container) -> back_insert_iterator<C>
    {
        return back_insert_iterator<C>(container);
    }

    template <typename C>
    class front_insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
        C* container;

    public:
        using container_type = C;
        using difference_type = ptrdiff_t;

        explicit front_insert_iterator(C& container) : container(&container) {}

        auto operator=(const typename C::value_type& val) -> front_insert_iterator&
        {
            container->push_front(val);
            return *this;
        }

        auto operator=(typename C::value_type&& val) -> front_insert_iterator&
        {
            container->push_front(std::move(val));
            return *this;
        }

        [[nodiscard]] auto operator*() -> front_insert_iterator& { return *this; }
        auto operator++() -> front_insert_iterator& { return *this; }
        auto operator++(int) -> front_insert_iterator { return *this; }
    };

    template <typename C>
    [[nodiscard]] inline auto front_inserter(C& c) -> front_insert_iterator<C>
    {
        return front_insert_iterator<C>(c);
    }

    template <typename C>
    class insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
    {
        using It = typename C::iterator;

    protected:
        C* container;
        It iter;

    public:
        using container_type = C;

        insert_iterator(C& container, It iter) : container(&container), iter(iter) {}

        auto operator=(const typename C::value_type& val) -> insert_iterator&
        {
            iter = container->insert(iter, val);
            ++iter;
            return *this;
        }

        auto operator=(typename C::value_type&& val) -> insert_iterator&
        {
            iter = container->insert(iter, std::move(val));
            ++iter;
            return *this;
        }

        [[nodiscard]] auto operator*() -> insert_iterator& { return *this; }
        auto operator++() -> insert_iterator& { return *this; }
        auto operator++(int) -> insert_iterator& { return *this; }
    };
} // namespace std

#endif
