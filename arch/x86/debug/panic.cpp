#include <cstddef>
#include <printf.h>
#include <kinit/boot_resource.h>

namespace debug
{
    inline constexpr const char* STACK_TYPES[] = {
        "KINIT",
        "IRQ",
        "SMP",
    };

    void panic(const char* c, bool crash)
    {
        std::printf("panic: %s\n", c);
        const char* mode = boot_resource::instance().is_smp() ? "smp" : "kinit";
        std::printf("stage = \033cc;%s\033\n", mode);
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

            std::printf("#%lu: 0x%016lx\n", n++, ret_addr);
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
