#ifndef __TTY_STARTER_DRIVE__
#define __TTY_STARTER_DRIVE__
#include "utils.h"
#include <array>
#include <cctype>
#define KDEBUG

namespace tty
{
    class romfont
    {
        const uint8_t font_width;
        const uint8_t font_height;
        void* font;

    public:
        constexpr romfont(uint8_t w, uint8_t h) : font_width(w), font_height(h), font(nullptr) {}
        constexpr romfont(uint8_t w, uint8_t h, void* n) : font_width(w), font_height(h), font(n) {}

        constexpr romfont(const romfont& f) = default;
        constexpr romfont(romfont&& f) = default;

        constexpr uint8_t width() const { return font_width; }
        constexpr uint8_t height() const { return font_height; }
        constexpr std::size_t bpc() const { return (font_width * font_height) >> 3; }
        constexpr void set_font(void* font) { this->font = font; }

        char* char_at(char ch) { return &((char*)font)[ch * bpc()]; }
    };

    struct rgb
    {
        uint8_t r, g, b;
    };

    inline constexpr rgb WHITE = {255, 255, 255};
    inline constexpr rgb BLACK = {0, 0, 0};
    inline constexpr rgb GREY = {127, 127, 127};
    inline constexpr rgb RED = {255, 0, 0};
    inline constexpr rgb GREEN = {0, 255, 0};
    inline constexpr rgb BLUE = {0, 0, 255};
    inline constexpr rgb YELLOW = {255, 255, 0};
    inline constexpr rgb CYAN = {0, 255, 255};
} // namespace tty

namespace driver
{
    class tty_startup_driver
    {
        bool is_command = false;
        bool is_text = false;
        bool is_escape = false;
        char current_command = 0;
        uint8_t index = 0;
        uint8_t chindex = 0;
        char command_buffer[32][8];

        friend void std::detail::putc(char ch);

        void handle_color1();

        using handler_type = void (tty_startup_driver::*)();

        static constexpr handler_type handlers[26][8] = {{}, {}, {&tty_startup_driver::handle_color1}};

        void putc_handle_escape(char c);

    protected:
        tty::rgb color = tty::WHITE;

    public:
        virtual void putc(char c) = 0;
        virtual ~tty_startup_driver() = 0;
    };

    void set_tty_startup_driver(tty_startup_driver*);
} // namespace driver

#endif
