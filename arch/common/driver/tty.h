#ifndef __TTY_STARTER_DRIVE__
#define __TTY_STARTER_DRIVE__
#include "utils.h"
#define KDEBUG

namespace tty
{
    class romfont
    {
        const uint8_t font_width;
        const uint8_t font_height;
        void* font;
    public:
        constexpr romfont(uint8_t w, uint8_t h)
            :  font_width(w), font_height(h), font(nullptr)
        {
        }
        constexpr romfont(uint8_t w, uint8_t h, void* n)
            :  font_width(w), font_height(h), font(n)
        {
        }

        constexpr romfont(const romfont& f) = default;
        constexpr romfont(romfont&& f) = default;

        constexpr uint8_t width() const { return font_width; }
        constexpr uint8_t height() const { return font_height; }
        constexpr std::size_t bpc() const { return font_width * font_height; }
        constexpr void set_font(void* font) { this->font = font; }

        char* char_at(char ch)
        {
            return &((char*) font)[ch * bpc()];
        }
    };

    struct rgb
    {
        uint8_t r, g, b;
    };
}

namespace driver
{
    class tty_startup_driver
    {
    public:
        virtual void putc(char c) = 0;
    };

    void set_tty_startup_driver(tty_startup_driver*);
} // namespace driver

#endif
