#include "spinlock.h"
#include <smp/smp.h>

namespace lock
{
    void spinlock::lock()
    {
        if constexpr (config::get_val<"debug.lock.spinlock_dep">)
        {
            while (!__sync_bool_compare_and_swap(&l, 0, smp::core_local::get().core_id + 1))
                ;
            __sync_synchronize();
        }
        else
        {
            while (!__sync_bool_compare_and_swap(&l, 0, 1))
                ;
            __sync_synchronize();
        }
    }

} // namespace lock
