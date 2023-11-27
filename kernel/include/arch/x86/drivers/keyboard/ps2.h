#include <asm/asm_cpp.h>
#include <cstdint>
#include <klog/klog.h>
#include <utility>
std::pair<std::uint8_t, bool> decode_keyboard_scancode(std::uint8_t scancode, const std::uint8_t* translation_table, const std::size_t table_size,
                                                       const std::uint8_t FLAG_RELEASED)
{
    std::uint8_t decoded_keypress = 0;
    bool is_pressed = !(scancode & FLAG_RELEASED); // false means released
    // klog::log("SCANCODE:: %2X, %2X", scancode, scancode & (~FLAG_RELEASED));
    // klog::log("SIZE:: %d", table_size);
    if ((scancode & (~FLAG_RELEASED)) < table_size)
    {
        klog::log("HERE!!!!!");
        decoded_keypress = translation_table[scancode & (~FLAG_RELEASED)];
    }
    /*if (scancode < (sizeof(translation_table) / sizeof(us_qwerty_translation[0])))
    {
        decoded_keypress = translation_table[scancode];
    }
    else if(scancode -FLAG_RELEASED>=0 && scancode-FLAG_RELEASED <(sizeof(translation_table)/sizeof(translation_table)))
    {
        decoded_keypress
    }*/
    // klog::log("%2X, %d", decoded_keypress, is_pressed);
    return {decoded_keypress, is_pressed};
}
