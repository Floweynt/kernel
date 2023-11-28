#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <misc/user.h>

namespace user
{
    class file_desc;

    class fd_operations : public std::simple_refcountable<std::uint64_t, fd_operations>
    {
    public:
        enum seek_type
        {
            SEEK_START,
            SEEK_CURR,
            SEEK_END
        };

        [[nodiscard]] virtual auto read(file_desc& instance, user_pointer<const std::uint8_t> buffer, std::size_t count) const -> std::ssize_t = 0;
        [[nodiscard]] virtual auto write(file_desc& instance, user_pointer<std::uint8_t> buffer, std::size_t count) const -> std::ssize_t = 0;
        [[nodiscard]] virtual auto close(file_desc& instance) const -> std::ssize_t = 0;
        [[nodiscard]] virtual auto seek(file_desc& instance, std::ssize_t offset, seek_type type) const -> std::ssize_t = 0;

        virtual ~fd_operations() = default;
    };

    class file_desc : public std::simple_refcountable<std::uint64_t, file_desc>
    {
        std::refcounted<const fd_operations> operations;

    public:
        inline auto read(user_pointer<const std::uint8_t> buffer, std::size_t count) { return operations->read(*this, buffer, count); }
        inline auto write(user_pointer<std::uint8_t> buffer, std::size_t count) { return operations->write(*this, buffer, count); }
        inline auto close() { return operations->close(*this); }
        inline auto seek(std::ssize_t offset, fd_operations::seek_type type) { return operations->seek(*this, offset, type); }
    };
} // namespace user
