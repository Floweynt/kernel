#include "../bits/user_implement.h"
#include "../bits/utils.h"
#include <cstring>
#include <new>

namespace std
{
    char* strcpy(char* __restrict dest, const char* __restrict src) { return (char*)memcpy(dest, src, strlen(src) + 1); }

    char* strncpy(char* __restrict dest, const char* __restrict src, size_t n)
    {
        return (char*)memcpy(dest, src, detail::min(strlen(src), n) + 1);
    }

    char* strcat(char* __restrict dest, const char* __restrict src)
    {
        strcpy(dest + strlen(dest), src);
        return dest;
    }

    char* strncat(char* __restrict dest, const char* __restrict src, size_t n)
    {
        strncpy(dest + strlen(dest), src, n);
        return dest;
    }

    [[gnu::malloc]] char* strdup(const char* s)
    {
        size_t n = strlen(s);
        return (char*)memcpy(new char[n + 1], s, n + 1);
    }

    [[gnu::malloc]] char* strndup(const char* s, size_t count)
    {
        size_t n = detail::min(strlen(s), count);
        return (char*)memcpy(new char[n + 1], s, n + 1);
    }

    size_t strlen(const char* str)
    {
#ifdef __x86_64__
        const char* s = str;
        const size_t* ptr;

        constexpr size_t himagic = 0x8080808080808080;
        constexpr size_t lomagic = 0x0101010101010101;

        for (; (size_t)s / alignof(size_t) != 0; s++)
            if (*s == '\0')
                return s - str;

        ptr = (size_t*)s;

        while (true)
        {
            size_t longword = *ptr++;

            if (((longword - lomagic) & ~longword & himagic) != 0)
            {
                const char* cp = (const char*)(ptr - 1);

                if (cp[0] == 0)
                    return cp - str;
                if (cp[1] == 0)
                    return cp - str + 1;
                if (cp[2] == 0)
                    return cp - str + 2;
                if (cp[3] == 0)
                    return cp - str + 3;
                if (sizeof(longword) > 4)
                {
                    if (cp[4] == 0)
                        return cp - str + 4;
                    if (cp[5] == 0)
                        return cp - str + 5;
                    if (cp[6] == 0)
                        return cp - str + 6;
                    if (cp[7] == 0)
                        return cp - str + 7;
                }
            }
        }
#endif
    }

    int strcmp(const char* s1, const char* s2)
    {
        const unsigned char* u1 = (const unsigned char*)s1;
        const unsigned char* u2 = (const unsigned char*)s2;
        unsigned char c1, c2;

        do
        {
            c1 = (unsigned char)*u1++;
            c2 = (unsigned char)*u2++;
            if (c1)
                return c1 - c2;
        } while (c1 == c2);
        return 0;
    }

    int strncmp(const char* s1, const char* s2, size_t n)
    {
        // budget vectorization
        // stolen from glibc:
        // https://github.com/zerovm/glibc/blob/master/string/strncmp.c
        unsigned char c1 = '\0';
        unsigned char c2 = '\0';

        if (n >= 4)
        {
            size_t n4 = n >> 2;
            do
            {
                c1 = (unsigned char)*s1++;
                c2 = (unsigned char)*s2++;
                if (c1 == '\0' || c1 != c2)
                    return c1 - c2;
                c1 = (unsigned char)*s1++;
                c2 = (unsigned char)*s2++;
                if (c1 == '\0' || c1 != c2)
                    return c1 - c2;
                c1 = (unsigned char)*s1++;
                c2 = (unsigned char)*s2++;
                if (c1 == '\0' || c1 != c2)
                    return c1 - c2;
                c1 = (unsigned char)*s1++;
                c2 = (unsigned char)*s2++;
                if (c1 == '\0' || c1 != c2)
                    return c1 - c2;
            } while (--n4 > 0);
            n &= 3;
        }

        while (n > 0)
        {
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            n--;
        }

        return c1 - c2;
    }

    char* strchr(char* s, int c) { return (char*)strchr((const char*)s, c); }

    const char* strchr(const char* s, int c_in)
    {
        // taken from https://github.com/zerovm/glibc/blob/master/string/strchr.c
#ifdef __x86_64__
        constexpr size_t magic_bits = 0x7efefefefefefeff;
        const unsigned char* char_ptr;
        const size_t* longword_ptr;
        size_t longword, charmask;
        unsigned char c = (unsigned char)c_in;

        for (char_ptr = (const unsigned char*)s; ((size_t)char_ptr & (sizeof(longword) - 1)) != 0; ++char_ptr)
        {
            if (*char_ptr == c)
                return (char*)char_ptr;
            else if (!*char_ptr)
                return nullptr;
        }

        longword_ptr = (size_t*)char_ptr;
        charmask = c | (c << 8);
        charmask |= charmask << 16;
        charmask |= (charmask << 16) << 16;

        while (true)
        {
            longword = *longword_ptr++;

            if ((((longword + magic_bits) ^ ~longword) & ~magic_bits) != 0 ||

                ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask)) & ~magic_bits) != 0)
            {
                const unsigned char* cp = (const unsigned char*)(longword_ptr - 1);

                if (*cp == c)
                    return (char*)cp;
                else if (*cp == '\0')
                    return nullptr;
                if (*++cp == c)
                    return (char*)cp;
                else if (*cp == '\0')
                    return nullptr;
                if (*++cp == c)
                    return (char*)cp;
                else if (*cp == '\0')
                    return nullptr;
                if (*++cp == c)
                    return (char*)cp;
                else if (*cp == '\0')
                    return nullptr;
                if (sizeof(longword) > 4)
                {
                    if (*++cp == c)
                        return (char*)cp;
                    else if (*cp == '\0')
                        return nullptr;
                    if (*++cp == c)
                        return (char*)cp;
                    else if (*cp == '\0')
                        return nullptr;
                    if (*++cp == c)
                        return (char*)cp;
                    else if (*cp == '\0')
                        return nullptr;
                    if (*++cp == c)
                        return (char*)cp;
                    else if (*cp == '\0')
                        return nullptr;
                }
            }
        }

        return nullptr;
#endif
    }

    char* strrchr(char* s, int c) { return (char*)strrchr((const char*)s, c); }

    const char* strrchr(const char* s, int c)
    {
        const char* found = nullptr;
        c = (unsigned char)c;

        if (!c)
            return strchr(s, 0);

        const char* p = nullptr;
        while ((p = strchr(s, c)) != NULL)
        {
            found = p;
            s = p + 1;
        }

        return found;
    }

    size_t strspn(const char* s, const char* accept)
    {
        size_t count = 0;

        for (const char* p = s; *p; ++p)
        {
            for (const char* a = accept; *a; a++)
            {
                if (*p == *a)
                    break;
                if (!*a)
                    return count;
                else
                    count++;
            }
        }
        return count;
    }

    size_t strcspn(const char* s, const char* reject)
    {
        size_t count = 0;

        while (*s)
        {
            if (!strchr(reject, *s++))
                ++count;
            else
                return count;
        }

        return count;
    }

    char* strpbrk(char* s, const char* accept) { return (char*)strpbrk((const char*)s, accept); }

    const char* strpbrk(const char* s, const char* accept)
    {
        while (*s)
        {
            const char* a = accept;
            while (*a)
                if (*a++ == *s)
                    return (char*)s;
            s++;
        }
        return nullptr;
    }

    char* strstr(char* s, const char* target);
    const char* strstr(const char* s, const char* target);

    char* strtok(char* __restrict s, const char* __restrict delim);

    void* memchr(void* s, int c, size_t n);
    const void* memchr(const void* s, int c, size_t n);

    int memcmp(const void* s1, const void* s2, size_t n);
    void* memset(void* s, int c, size_t n)
    {
        c = c & 0xff;
        size_t v = c | (c << 8) | (c << 16) | (c << 24);
        char* b = (char*)s;
        if constexpr (sizeof(size_t) == 8)
            v |= ((size_t)c << 32) | ((size_t)c << 40) | ((size_t)c << 48) | ((size_t)c << 56);

        while (((size_t)b) % alignof(size_t) != 0 && n-- != 0)
            *b++ = c;

        if (n == 0)
            return s;

        size_t* sp = (size_t*)b;
        for (size_t i = 0; i < n / 8; i++)
            sp[i] = v;

        b = (char*)sp;
        for (size_t i = 0; i < n % 8; i++)
            b[i] = c;

        return s;
    }

    void* memcpy(void* __restrict dest, const void* __restrict src, size_t n)
    {
#ifdef __x86_64__
        // optimized implementation for x86
        char* d = (char*)dest;
        const char* s = (const char*)src;

        while (((size_t)d % alignof(size_t) != 0) && ((size_t)s % alignof(size_t) != 0) && n-- != 0)
            *d++ = *s++;

        if (n == 0)
            return dest;

        size_t* sd = (size_t*)d;
        const size_t* ss = (size_t*)s;

        size_t i = n / sizeof(size_t);
        while (i--)
            *sd++ = *ss++;

        n %= sizeof(size_t);

        d = (char*)sd;
        s = (const char*)ss;
        while (n--)
            *d++ = *s++;

        return dest;
#endif
    }

    void* memccpy(void* __restrict dest, const void* __restrict src, int c, size_t n);
    void* memmove(void* dest, const void* src, size_t n);
} // namespace std
