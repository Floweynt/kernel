#ifndef __ARCH_X86_DRIVER_TERMINAL_H__
#define __ARCH_X86_DRIVER_TERMINAL_H__
#include <cstddef>
#include <common/driver/tty.h>
#include <kinit/stivale2.h>

namespace driver
{

    class simple_tty : public tty_startup_driver
    {
        stivale2_struct_tag_framebuffer buffer;
        tty::romfont f;
        tty::rgb color;
        char* screen_buffer;
        std::size_t rotate_offset;
        std::size_t x;
        std::size_t y;

        void render_character(char chi, std::size_t x, std::size_t y);
        void scrollup();
    public:
        simple_tty(const stivale2_struct_tag_framebuffer& buffer, tty::romfont f);
        void putc(char) override;

        inline void* pixel_at(std::size_t x, std::size_t y)
        {
            return (void*)(buffer.framebuffer_addr + y * buffer.framebuffer_pitch + x * buffer.framebuffer_bpp);
        }

        constexpr std::size_t lines()
        {
            return buffer.framebuffer_height / f.height();
        }

        constexpr std::size_t cols()
        {
            return buffer.framebuffer_width / f.width();
        }

        inline char char_at(std::size_t i, std::size_t j)
        {
            i = (rotate_offset + i) % lines();
            return screen_buffer[i * cols() + j];
        }
    };
} // namespace driver

#endif
