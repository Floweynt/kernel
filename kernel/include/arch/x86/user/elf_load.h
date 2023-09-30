#pragma once
#include <process/process.h>

namespace user
{
    auto load_elf(void* elf_data, proc::thread& thread) -> bool;
}
