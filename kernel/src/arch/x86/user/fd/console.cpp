#include <user/fd/console.h>

namespace user::fd 
{
    auto console_fd::read(file_desc& instance, user_pointer<const std::uint8_t> buffer, std::size_t count) const -> std::ssize_t
    {
        // do we really care at this point? just fucking write it!
        // TODO: copy user space page tables
    
        return count;
    }
}
