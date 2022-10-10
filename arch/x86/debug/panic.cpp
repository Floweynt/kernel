#include <cstddef>
#include "debug.h"
#include <printf.h>
#include <kinit/boot_resource.h>

namespace debug
{
    inline constexpr const char* STACK_TYPES[] = {
        "KINIT",
        "IRQ",
        "SMP",
    };

    void panic(const char* msg, bool crash)
    {
        std::printf("panic: %s\n", msg);
        const char* stage = boot_resource::instance().is_smp() ? "smp" : "kinit";
        std::printf("stage = \033cc;%s\033\n", stage);
        std::printf("crash = %B\n", crash);
        std::size_t* base_ptr;

        asm volatile("movq %%rbp, %0" : "=g"(base_ptr) : : "memory");
        std::size_t n = 0;
        std::size_t stack_color = 0;
        while (true)
        {
            std::size_t old_bp = base_ptr[0];
            std::size_t ret_addr = base_ptr[1];
            if (!ret_addr)
                break;

            auto symbol = sym_for(ret_addr);
            std::printf("#%lu: 0x%016lx <\"%s\"+0x%08x>\n", n++, ret_addr, symbol.name, symbol.offset);
            if (old_bp < 0x1000)
            {
                stack_color = old_bp;
                break;
            }

            base_ptr = (std::size_t*)old_bp;
        }

        std::printf("stack type: \033cc;%s\033\n", stack_color < sizeof(STACK_TYPES) ? STACK_TYPES[stack_color] : "unknown");

        if(crash)
            while(1) { asm volatile ("hlt"); }
    }
}
