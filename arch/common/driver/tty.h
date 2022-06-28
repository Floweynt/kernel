// cSpell:ignore chindex, scrollup
#ifndef __TTY_STARTER_DRIVE__
#define __TTY_STARTER_DRIVE__
#include "utils.h"
#include <array>
#include <cctype>
#define KDEBUG

namespace tty
{
    /// \brief A font found in memory, typically hardcoded into the kernel binary
    /// Contains a bitmap font, of a specific dimension. The width and height are immutable, however the contained font can
    /// be mutated (only the font pointer, not data as that should be in ROM, which is immutable)
    class romfont
    {
        const uint8_t font_width;
        const uint8_t font_height;
        // pointer to the data that contains the actual font
        void* font;

    public:
        /// \brief Creates a font given the width and height
        /// \param w The width of the font
        /// \param h The height of the font
        /// Creates a font of \p width and \p height, but sets the font data to null.
        constexpr romfont(uint8_t w, uint8_t h) : font_width(w), font_height(h), font(nullptr) {}

        /// \brief Creates a font of size width and height, with font data pointed to by n
        /// \param w The width of the font
        /// \param h The height of the font
        /// \param n The raw binary data of the font
        constexpr romfont(uint8_t w, uint8_t h, void* n) : font_width(w), font_height(h), font(n) {}

        /// \brief Copy constructor
        /// \param f The font to copy from
        /// Creates a new font with the same properties as f. The copy constructor does not copy the data pointed to by the
        /// internal font pointer
        constexpr romfont(const romfont& f) = default;

        /// \brief Move constructor
        constexpr romfont(romfont&& f) = default;

        /// \brief Obtains the width of the font
        /// \return The width of the font
        constexpr uint8_t width() const { return font_width; }

        /// \brief Obtains the height of the font
        /// \return The height of the font
        constexpr uint8_t height() const { return font_height; }

        /// \brief Obtains the bytes per character
        /// \return The bytes per character
        constexpr std::size_t bpc() const { return (font_width * font_height) >> 3; }

        /// \brief Sets the font pointer
        /// \param font The new fount
        constexpr void set_font(void* font) { this->font = font; }

        // \brief Obtains the start of a character in the romfont data
        // \param ch The character who's offset will be calculated
        // \return A pointer to the start of character font data
        char* char_at(char ch) { return &((char*)font)[ch * bpc()]; }
    };

    /// \brief A representation of an 8-bit RGB color
    /// A literal type that represnets a 8-bit RGB color
    struct rgb
    {
        uint8_t r, g, b;
    };

    // Commonly used colors, as an rgb instance
    inline constexpr rgb WHITE = {255, 255, 255};
    inline constexpr rgb BLACK = {0, 0, 0};
    inline constexpr rgb GREY = {127, 127, 127};
    inline constexpr rgb RED = {255, 0, 0};
    inline constexpr rgb GREEN = {0, 255, 0};
    inline constexpr rgb BLUE = {0, 0, 255};
    inline constexpr rgb YELLOW = {255, 255, 0};
    inline constexpr rgb CYAN = {0, 230, 230};
} // namespace tty

namespace driver
{
    /// \brief Virtual class that handles tty support
    /// This should only be used before actual video drivers are implemented, in which case the use of a more advanced
    /// driver is recommended. There is builtin support for color and other escape sequences, although not in the ANSI
    /// escape format.
    class tty_startup_driver
    {
        // The following members are states used for the escape sequence parser

        // Flags used by the escape sequence parser

        // Is the current character parsed a part of a command?
        bool is_command = false;

        // Is the current character part of the text to be formatted by the escape sequence
        bool is_text = false;

        // Current command name
        char current_command = 0;
        // Argument index into command_buffer
        uint8_t index = 0;
        // Character index into command_buffer[index]
        uint8_t chindex = 0;
        // Buffer used for parsing commands
        char command_buffer[8][32];

        // Handlers for the different escape commands
        void handle_color1();
        inline void handle_scrollup() { scrollup(); }
        inline void handle_rerender() { rerender(); }

        // Member-function typedef
        using handler_type = void (tty_startup_driver::*)();

        // A array of all the handler functions, which is indexed by currend_command - 'a'.
        // There are multiple version of the same handler function in order to implement overloading
        static constexpr handler_type handlers[26][8] = {
            {},
            {},
            {&tty_startup_driver::handle_color1},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {&tty_startup_driver::handle_rerender},
            {&tty_startup_driver::handle_scrollup},
        };

        // A implementation of putc that prints and also parses escape sequences
        // Escape sequence format:
        // The escape sequence begins and ends with \x1b.
        // The command character appears directly after the starting \x1b
        // It is then followed by a comma seperated list of arguments, up to 8 possible, and each of length 32 max
        // It is then followed by a semicolon, then the text to which the formatting will be applied to, then \x1b
        // Due to the nature of this format, multiple formatting is not supported
        //
        // In a more programmer-friendly format, where <> means required, and () means optional:
        // The command sequence is defined as:
        //    <command_char>(arg1,)...(arg8,)
        // The entire escape sequence can be defined as:
        //    \x1b<command_sequence>;<text>\x1b
        //
        // Future support for multiple formatting commands can be implemented with |
        // The \x1b may also be replaced with a more readable character
        friend void std::detail::putc(char ch);
        void putc_handle_escape(char c);

    protected:
        // The current state of the tty, set via putc_handle_escape
        tty::rgb fg_color = tty::WHITE;
        tty::rgb bg_color = tty::BLACK;

    public:
        /// \brief Writes a character to an appropriate target
        /// \param c The character to print
        /// Writes the character to a serial port, some section of memory, or draws the character to the screen depenging
        /// on the implementation
        virtual void putc(char c) = 0;

        /// \brief Scrolls the screen up if appropriate
        ///
        virtual void scrollup() = 0;
        /// \brief Re-renders the screen if appropriate
        ///
        virtual void rerender() = 0;

        virtual ~tty_startup_driver() = 0;
    };

    /// \brief Sets the tty_startup_driver used for putc (and indirectly, printf)
    /// \param d The new instance of a tty_startup_driver
    void set_tty_startup_driver(tty_startup_driver* d);
} // namespace driver

#endif
