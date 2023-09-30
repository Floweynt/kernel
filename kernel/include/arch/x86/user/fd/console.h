
#include <user/fd/fd.h>

namespace user::fd
{
    class console_fd final : fd_operations
    {
        [[nodiscard]] virtual auto read(file_desc& instance, user_pointer<const std::uint8_t> buffer, std::size_t count) const -> std::ssize_t = 0;
        [[nodiscard]] virtual auto write(file_desc& instance, user_pointer<std::uint8_t> buffer, std::size_t count) const -> std::ssize_t = 0;
        [[nodiscard]] virtual auto close(file_desc& instance) const -> std::ssize_t = 0;
        [[nodiscard]] virtual auto seek(file_desc& instance, std::ssize_t offset, seek_type type) const -> std::ssize_t = 0;
    };

    void init_console();
} // namespace user::fd::console
