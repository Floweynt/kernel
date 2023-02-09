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

        back_insert_iterator& operator=(const typename C::value_type& __value)
        {
            container->push_back(__value);
            return *this;
        }

        back_insert_iterator& operator=(typename C::value_type&& v)
        {
            container->push_back(std::move(v));
            return *this;
        }

        [[nodiscard]] back_insert_iterator& operator*() { return *this; }
        back_insert_iterator& operator++() { return *this; }
        back_insert_iterator operator++(int) { return *this; }
    };

    template <typename C>
    [[nodiscard]] inline back_insert_iterator<C> back_inserter(C& c)
    {
        return back_insert_iterator<C>(c);
    }

    template <typename C>
    class front_insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
        C* container;

    public:
        typedef C container_type;
        using difference_type = ptrdiff_t;

        explicit front_insert_iterator(C& c) : container(&c) {}

        front_insert_iterator& operator=(const typename C::value_type& val)
        {
            container->push_front(val);
            return *this;
        }

        front_insert_iterator& operator=(typename C::value_type&& val)
        {
            container->push_front(std::move(val));
            return *this;
        }

        [[nodiscard]] front_insert_iterator& operator*() { return *this; }
        front_insert_iterator& operator++() { return *this; }
        front_insert_iterator operator++(int) { return *this; }
    };

    template <typename C>
    [[nodiscard]] inline front_insert_iterator<C> front_inserter(C& c)
    {
        return front_insert_iterator<C>(c);
    }

    template <typename C>
    class insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
    {
        typedef typename C::iterator It;

    protected:
        C* container;
        It iter;

    public:
        typedef C container_type;

        insert_iterator(C& c, It i) : container(&i), iter(i) {}

        insert_iterator& operator=(const typename C::value_type& val)
        {
            iter = container->insert(iter, val);
            ++iter;
            return *this;
        }

        insert_iterator& operator=(typename C::value_type&& val)
        {
            iter = container->insert(iter, std::move(val));
            ++iter;
            return *this;
        }

        [[nodiscard]] insert_iterator& operator*() { return *this; }
        insert_iterator& operator++() { return *this; }
        insert_iterator& operator++(int) { return *this; }
    };
} // namespace std

#endif
