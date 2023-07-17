#include "spinlock.h"
#include <smp/smp.h>
namespace lock
{
    void spinlock::lock()
    {
        if constexpr (config::get_val<"debug.lock.spinlock_dep">)
        {
            auto value = smp::core_local::get().core_id + 1;

            while (true)
            {
                if (!l.exchange(value, std::memory_order_acquire))
                {
                    break;
                }
                while (l.load(std::memory_order_relaxed))
                {
                    __builtin_ia32_pause();
                }
            }
        }
        else
        {
            while (true)
            {
                if (!l.exchange(1, std::memory_order_acquire))
                {
                    break;
                }

                while (l.load(std::memory_order_relaxed))
                {
                    __builtin_ia32_pause();
                }
            }
        }
    }
} // namespace lock
