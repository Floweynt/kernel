#ifndef __LIB_MEMORY__
#define __LIB_MEMORY__

#include <cstddef>

void memcpy(void* dest, const void* src, size_t n);
void memcpy(void* dest, const void* src, size_t n, size_t skipped);

int memcmp(const void* str1, const void* str2, size_t n);

void* memmove(void* dest, const void *src, size_t n);

void* memset(void *dest, int val, size_t n);
void* memset(void *dest, int val, size_t n, size_t skipped);
#endif