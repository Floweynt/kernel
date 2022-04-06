#include "terminal.h"
#include <cstring>
namespace driver
{
    simple_tty::simple_tty(const stivale2_struct_tag_framebuffer& buffer, tty::romfont f) : buffer(buffer), f(f)
    {
        if(this->buffer.framebuffer_addr < 0xffff800000000000)
            this->buffer.framebuffer_addr += 0xffff800000000000;
    }

    void simple_tty::scrollup()
    {
        std::memset(screen_buffer, 0, cols());
        rotate_offset += cols();

        // TODO: impl 
    }

    void simple_tty::render_character(char c, std::size_t x, std::size_t y)
    {
        uint64_t px = (((((1ull << buffer.red_mask_size) - 1) * color.r) / 255) << buffer.red_mask_shift) |
                      (((((1ull << buffer.green_mask_size) - 1) * color.g) / 255) << buffer.green_mask_shift) |
                      (((((1ull << buffer.blue_mask_size) - 1) * color.b) / 255) << buffer.blue_mask_shift);
        
        char* current_pixel = (char*)pixel_at(x * f.width(), y * f.height());
        char* current_char = (char*)f.char_at(c);
        uint8_t bit_index = 0;
        for(int i = 0; i < f.height(); i++)
        {
            for(int j = 0; j < f.width(); j++)
            {
                if(*current_char & (1<< bit_index))
                {
                    char* pixel_buffer = (char*)&px;
                    for(int i = 0; i < buffer.framebuffer_bpp; i++)
                        *current_pixel++ = *pixel_buffer++;
                }

                bit_index++;
                current_char += bit_index / 8;
                bit_index %= 8;
            }

            current_pixel += (buffer.framebuffer_pitch - f.width() * buffer.framebuffer_bpp);
        }
    }

    void simple_tty::putc(char c)
    {
        switch(c)
        {
        case ' ':
            x++;
            break;
        case '\n':
            y++;
            break;
        default:
            render_character(c);
        }

        if(x == cols())
        {
            x = 0;
            y++;
        }

        if(y == lines())
        {
            y--;
            
            


        }
    }
}
