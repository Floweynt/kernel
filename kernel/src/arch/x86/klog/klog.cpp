#include <config.h>
#include <klog/klog.h>
#include <sync/spinlock.h>

namespace klog::detail
{
    namespace
    {
        lock::spinlock lock;
    }

    auto get_klog_lock() -> lock::spinlock& { return lock; }
} // namespace klog::detail
