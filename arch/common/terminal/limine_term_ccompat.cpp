#include <cstddef>
#include <cstring>

extern "C" void* memset(void *s, int c, std::size_t n) { return std::memset(s,c,n); }
extern "C" void* memcpy(void* a, const void* b, std::size_t c) { return std::memcpy(a, b, c); }
