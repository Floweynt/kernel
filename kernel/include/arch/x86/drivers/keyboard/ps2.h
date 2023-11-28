#include <asm/asm_cpp.h>
#include <cstdint>
#include <klog/klog.h>
std::uint8_t decode_keyboard_scancode(std::uint8_t scancode, const std::uint8_t* translation_table, const std::uint8_t* translation_table_end,
                                      const std::uint8_t flag_pressed)
{
    std::uint8_t decoded_keystroke = 0;
    // bool is_pressed = !(scancode & flag_pressed); // false means released
    //  klog::log("SCANCODE:: %2X, %2X", scancode, scancode & (~flag_pressed));
    //  klog::log("SIZE:: %d", table_size);
    std::uint8_t key_scancode = scancode & (~flag_pressed); // Stripped of press flag
    if ((scancode & (~flag_pressed)) < ((translation_table_end - translation_table) / sizeof(translation_table[0])))
    {
        // klog::log("HERE!!!!!");
        decoded_keystroke = translation_table[scancode & (~flag_pressed)];
    }
    // klog::log("%2X, %d", decoded_keystroke, is_pressed);
    return decoded_keystroke;
}
void poll_keystroke(const std::uint8_t* translation_table, const std::uint8_t* translation_table_end)
{
    auto prev_msg = 0;
    while (true)
    {
        auto scancode = inb(ioports::KEYBOARD);
        if (scancode == prev_msg)
            continue;
        else
            prev_msg = scancode;
        // klog::log("Encoded Keypress: %02X, %02X", scancode, prev_msg);
        /* 0x1B = ESCAPE; 0x08 = BACKSPACE; 0x09 = HORIZONAL TAB, used to represent TAB
         * 0x0D = CARRIAGE RETURN, used to represent ENTER
         * 0x11 = DEVICE CONTROL 1, used to represent LEFT CONTROL; 0x0E = SHIFT OUT, used to represent LEFT SHIFT
         * 0x0F = SHIFT IN, used to represent RIGHT SHIFT; 0x13 = DEVICE CONTROL 3, used to represent LEFT ALT
         * 0x02 = START OF TEXT, used to represent CAPS LOCK
         * TODO: Handle F1-F12, Numlock and Scroll Lock, multimedia keys
         * also Home, Page Up, etc
         * TODO eventually: distinguish Keypad / (0xE0, 0x35) and enter (0xE0, 0x1C) from normal, and RIGHT CTRL and ALT from LEFT
         */
        std::uint8_t decoded_keystroke;
        static constexpr std::uint8_t FLAG_PRESSED = 0x80;
        bool is_pressed = !(scancode & FLAG_PRESSED);

        decoded_keystroke = decode_keyboard_scancode(
            scancode, translation_table, translation_table_end, FLAG_PRESSED);
        if (decoded_keystroke != 0)
        {
            // THIS IS A TEMPORARY SWITCH CASE FOR DEBUGGING, TODO: DELETE IT AND ACTUALLY HANDLE STUFF
            char output_str[15] = "";
            switch (decoded_keystroke)
            {
            case 0x1B:
                std::strcpy(output_str, "ESCAPE");
                break;
            case 0x08:
                std::strcpy(output_str, "BACKSPACE");
                break;
            case 0x09:
                std::strcpy(output_str, "TAB");
                break;
            case 0x0D:
                std::strcpy(output_str, "ENTER");
                break;
            case 0x11:
                std::strcpy(output_str, "CTRL");
                break;
            case 0x0E:
                std::strcpy(output_str, "LEFT SHIFT");
                break;
            case 0x0F:
                std::strcpy(output_str, "RIGHT SHIFT");
                break;
            case 0x13:
                std::strcpy(output_str, "ALT");
                break;
            case 0x02:
                std::strcpy(output_str, "CAPS LOCK");
                break;
            default:
                std::strcpy(output_str, (char[2]){(char)decoded_keystroke, '\0'});
                break;
            }
            klog::log("Detected Keyboard Input: %s %s", output_str, is_pressed ? "pressed" : "released");
        }
    }
}
