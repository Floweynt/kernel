#ifndef __ARCH_X86_CONTEXT_H__
#define __ARCH_X86_CONTEXT_H__
#include <cstdint>
#include "config.h"

struct context
{
    uint64_t rip;
    uint64_t rflags;
    uint64_t rsp;

    uint64_t rgp[16];

    uint64_t cs;
    uint64_t ss;

    inline void load_ctx(void* stackpos)
    {
        uint64_t* stack = (uint64_t*)stackpos;
        rflags = *stack--;
        uint64_t selector = *stack--;
        if(selector == KERNEL_SPACE_CS)
        {
            cs = selector;
            ss = 0;
        }
        else
        {
            ss = selector;
            rsp = *stack--;
            cs = *stack--;
        }

        rip = *stack--;
        stack--;
        stack--;

        for(int i = 0; i < 16; i++)
            rgp[i] = *stack--;
    }
};
#endif
