#include "return_to_context.h"
#include "asm_cpp.h"
#include <mm/mm.h>
#include <process/context.h>

extern "C" [[noreturn]] void _return_to_context_asm(proc::context* ptr);

extern "C" [[noreturn]] void return_to_context(proc::context* ptr)
{
    write_cr3(mm::make_physical(ptr->cr3));
    _return_to_context_asm(ptr);
}
