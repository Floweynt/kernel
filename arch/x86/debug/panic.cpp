#include "debug.h"
#include "klog/klog.h"
#include "process/context.h"
#include <cstddef>
#include <kinit/boot_resource.h>
#include <printf.h>
#include <tty/tty.h>
#include <utility>

namespace debug
{
    inline constexpr const char* STACK_TYPES[] = {
        "KINIT",
        "IRQ",
        "SMP",
    };

    void panic(const char* msg, bool crash)
    {
        klog::log(RED("panic") ": %s\n", msg);
        const char* stage = boot_resource::instance().is_smp() ? "smp" : "kinit";
        klog::log("stage = " CYAN("%s") "\n", stage);
        klog::log("crash = %B\n", crash);
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
            klog::log("#%lu: 0x%016lx <\"%s\"+0x%08x>\n", n++, ret_addr, symbol.name, symbol.offset);
            if (old_bp < 0x1000)
            {
                stack_color = old_bp;
                break;
            }

            base_ptr = (std::size_t*)old_bp;
        }

        klog::log("stack type: " CYAN("%s") "\n",
                    stack_color < sizeof(STACK_TYPES) ? STACK_TYPES[stack_color] : "unknown");

        if (crash)
            std::halt();
    }

    void log_register(proc::context* ctx)
    {
        klog::log("cs=0x%lx | sp=0x%lx\n", ctx->cs, ctx->ss);
        klog::log("rflags=0x%lx\n", ctx->rflags);
        klog::log("rip=0x%016lx\n", ctx->rip);
        klog::log("rsp=0x%016lx\n", ctx->rip);
        klog::log("GP registers:\n");
        for (std::size_t i = 0; i < 5; i++)
            klog::log("%3s=0x%016lx   %3s=0x%016lx   %3s=0x%016lx\n", proc::context::REGISTER_NAMES[i], ctx->rgp[i],
                      proc::context::REGISTER_NAMES[i + 1], ctx->rgp[i + 1], proc::context::REGISTER_NAMES[i + 2],
                      ctx->rgp[i + 2]);
    }

} // namespace debug
