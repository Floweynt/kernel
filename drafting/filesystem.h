#include <bits/stdc++.h>

class disk_driver
{

};


typedef native_word_t int;

class file_handle
{
public:
    enum class type
    {
        FILE,
        SOCKET,
    };

    enum class prop
    {
        SEEKABLE = 0x1
    };

    virtual type ftype() = 0;
    virtual uint32_t get_prop() = 0;
    virtual void read() = 0;
};
