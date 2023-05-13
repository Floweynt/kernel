#include <cstddef>
#include <cstring>

extern "C" auto memset(void *str, int val, std::size_t len) -> void* { return std::memset(str,val,len); }
extern "C" auto memcpy(void* dest, const void* src, std::size_t len) -> void* { return std::memcpy(dest, src, len); }
