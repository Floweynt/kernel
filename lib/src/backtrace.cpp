#include <backtrace.h>

namespace std
{
    size_t backtrace(size_t skip, size_t count, void** buf, size_t* base_ptr)
    {
        asm volatile("movq %%rbp, %0" : "=g"(base_ptr) : : "memory");
        size_t n = 0;
        while (true)
        {
            size_t old_bp = base_ptr[0];
            size_t ret_addr = base_ptr[1];
            if (!ret_addr)
                break;

            if (skip != 0)
                skip--;
            else if (count != 0)
            {
                n++;
                *buf++ = (void*)ret_addr;
                count--;
            }

            if (!old_bp)
                break;

            base_ptr = (size_t*)old_bp;
        }

        return n;
    }
} // namespace std
