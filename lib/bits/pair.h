#ifndef __NOSTDLIB_BITS_PAIR_H__
#define __NOSTDLIB_BITS_PAIR_H__

namespace std
{
    template <typename T1, typename T2>
    struct pair
    {
        using first_type = T1;
        using second_type = T2;

        first_type first;
        second_type second;
    };

    template <typename T1, typename T2>
    struct compressed_pair
    {
        using first_type = T1;
        using second_type = T2;

        [[no_unique_address]] first_type first;
        [[no_unique_address]] second_type second;
    };
} // namespace std

#endif
