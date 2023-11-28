#include <asm/asm_cpp.h>
#include <cstdint>
#include <klog/klog.h>
std::uint8_t decode_keyboard_scancode(std::uint8_t scancode, const std::uint8_t* translation_table, const std::uint8_t * translation_table_end,
                                      const std::uint8_t FLAG_RELEASED)
{
    std::uint8_t decoded_keypress = 0;
    // bool is_pressed = !(scancode & FLAG_RELEASED); // false means released
    //  klog::log("SCANCODE:: %2X, %2X", scancode, scancode & (~FLAG_RELEASED));
    //  klog::log("SIZE:: %d", table_size);
    if ((scancode & (~FLAG_RELEASED)) < ((translation_table_end-translation_table)/sizeof(translation_table[0])))
    {
        // klog::log("HERE!!!!!");
        decoded_keypress = translation_table[scancode & (~FLAG_RELEASED)];
    }
    // klog::log("%2X, %d", decoded_keypress, is_pressed);
    return decoded_keypress;
}
