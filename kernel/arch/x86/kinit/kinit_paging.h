#ifndef __ARCH_X86_KINIT_PAGING_H__
#define __ARCH_X86_KINIT_PAGING_H__
#include "paging/paging.h"
#include "asm/asm_cpp.h"

namespace paging
{
    extern page_table root_table;
    extern page_table kernel_l2;
    extern page_table kernel_l3;

    extern page_table identity_l2;
    extern page_table identity_l3;
}

#endif
