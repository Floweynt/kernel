#ifndef __ARCH_COMMON_TERMINAL_C_COMPAT_H__
#define __ARCH_COMMON_TERMINAL_C_COMPAT_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#define NULL ((void*) 0)
#endif

typedef uint64_t size_t;
typedef uint64_t uintptr_t;
#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *, int, size_t);
void *memcpy(void *, const void *, size_t);

#ifdef __cplusplus
}
#endif

#endif
