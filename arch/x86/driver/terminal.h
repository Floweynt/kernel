#ifndef __ARCH_X86_DRIVER_TERMINAL_H__
#define __ARCH_X86_DRIVER_TERMINAL_H__
#include <common/driver/tty.h>
#include <cstddef>
#include <kinit/stivale2.h>
#include <sync/spinlock.h>

namespace driver
{
    class simple_tty : public tty_startup_driver
    {
        struct screen_character
        {
            char ch;
            tty::rgb color;
        };

        stivale2_struct_tag_framebuffer buffer;
        tty::romfont f;
        screen_character* screen_buffer;
        std::size_t rotate_offset;
        std::size_t x;
        std::size_t y;

        void render_character(char ch, std::size_t x, std::size_t y);

    public:
        simple_tty(const stivale2_struct_tag_framebuffer& buffer, tty::romfont f);
        void putc(char) override;

        inline void* pixel_at(std::size_t x, std::size_t y)
        {
            return (void*)(buffer.framebuffer_addr + y * buffer.framebuffer_pitch + x * buffer.framebuffer_bpp);
        }

        /// \brief Obtains the amount of lines in this terminal
        /// \return The amount of lines
        constexpr std::size_t lines() { return buffer.framebuffer_height / f.height(); }

        /// \brief Obtains the amount of columns in this terminal
        /// \return The amount of columns
        constexpr std::size_t cols() { return buffer.framebuffer_width / f.width(); }

        // \brief Obtains the reference to a `screen_character` at \p i, \p j
        /// \return The amount of columns
        inline screen_character& char_at(std::size_t i, std::size_t j)
        {
            j = (rotate_offset + j) % lines();
            return screen_buffer[i * lines() + j];
        }

        void scrollup() override;
        void rerender() override;

        ~simple_tty();
    };
} // namespace driver

#endif
