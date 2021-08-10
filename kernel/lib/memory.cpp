#include <cstddef>

void* memcpy(void* dest, const void* src, size_t n)
{
    char* ch_dest = (char*) dest;
    const char* ch_src = (const char*) src;
    while(n--)
        *ch_dest++ = *ch_src++;
    return dest;
}

int memcmp (const void* str1, const void* str2, size_t n)
{
    const unsigned char* s1 = (const unsigned char*)str1;
    const unsigned char* s2 = (const unsigned char*)str2;
    while (n--)
    {
        if (*s1++ != *s2++)
            return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}

void* memmove (void* dest, const void *src, size_t n)
{
    char* d = (char*) dest;
    const char* s = (const char*)src;
    if (d < s)
        while (n--)
            *d++ = *s++;
    else
    {
        char* lasts = (char*)s + (n - 1);
        char* lastd = d + (n - 1);
        while (n--)
            *lastd-- = *lasts--;
    }
    return dest;
}

void* memset (void *dest, int val, size_t n)
{
    unsigned char *ptr = (unsigned char*)dest;
    while (n--)
        *ptr++ = val;
    return dest;
}

// not standard
void* memcpy(void* dest, const void* src, size_t n, size_t skipped)
{
    char* ch_dest = (char*) dest;
    const char* ch_src = (const char*) src;
    while(n--)
    {
        *ch_dest = *ch_src;
        ch_dest += skipped;
        ch_src += skipped;
    }
    return dest;
}

void* memset (void *dest, int val, size_t n, size_t skipped)
{
    unsigned char *ptr = (unsigned char*)dest;
    while (n--)
    {
        *ptr = val;
        ptr += skipped;
    }
    return dest;
}
