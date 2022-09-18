#ifndef __ARCH_COMMON_TTY_H__
#define __ARCH_COMMON_TTY_H__
#include <cstdint>
#include <cstddef>

namespace tty
{
    /// \brief A font found in memory, typically hardcoded into the kernel binary
    /// Contains a bitmap font, of a specific dimension. The width and height are immutable, however the contained font can
    /// be mutated (only the font pointer, not data as that should be in ROM, which is immutable)
    class romfont
    {
        const std::uint8_t font_width;
        const std::uint8_t font_height;
        // pointer to the data that contains the actual font
        void* font;

    public:
        /// \brief Creates a font given the width and height
        /// \param w The width of the font
        /// \param h The height of the font
        /// Creates a font of \p width and \p height, but sets the font data to null.
        constexpr romfont(std::uint8_t w, std::uint8_t h) : font_width(w), font_height(h), font(nullptr) {}

        /// \brief Creates a font of size width and height, with font data pointed to by n
        /// \param w The width of the font
        /// \param h The height of the font
        /// \param n The raw binary data of the font
        constexpr romfont(std::uint8_t w, std::uint8_t h, void* n) : font_width(w), font_height(h), font(n) {}

        /// \brief Copy constructor
        /// \param f The font to copy from
        /// Creates a new font with the same properties as f. The copy constructor does not copy the data pointed to by the
        /// internal font pointer
        constexpr romfont(const romfont& f) = default;

        /// \brief Move constructor
        constexpr romfont(romfont&& f) = default;

        /// \brief Obtains the width of the font
        /// \return The width of the font
        constexpr std::uint8_t width() const { return font_width; }

        /// \brief Obtains the height of the font
        /// \return The height of the font
        constexpr std::uint8_t height() const { return font_height; }

        /// \brief Obtains the bytes per character
        /// \return The bytes per character
        constexpr std::size_t bpc() const { return (font_width * font_height) >> 3; }

        /// \brief Sets the font pointer
        /// \param font The new fount
        constexpr void set_font(void* font) { this->font = font; }

        // \brief Obtains the start of a character in the romfont data
        // \param ch The character who's offset will be calculated
        // \return A pointer to the start of character font data
        char* char_at(char ch) const { return &((char*)font)[ch * bpc()]; }
    };

    /// \brief A representation of an 8-bit RGB color
    /// A literal type that represnets a 8-bit RGB color
    struct rgb
    {
        std::uint8_t r, g, b;
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

#endif
