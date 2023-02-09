#ifndef __NOSTDLIB_BITS_SHARED_PTR_H__
#define __NOSTDLIB_BITS_SHARED_PTR_H__
#include "../include/utility"

namespace std
{
    template <typename T>
    class shared_ptr
    {
        struct data
        {
            size_t strong_ref = 0;
            size_t weak_ref = 0;
            void* data;
            void (*deleter)(void*);
        };

    public:
    };
} // namespace std

#endif
