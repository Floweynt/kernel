#include <array>
#include <asm/asm_cpp.h>
#include <cstdint>
#include <klog/klog.h>
#include <span>
namespace drivers::ps2
{
  namespace {
  constexpr auto TRANSLATION_TABLE_SIZE = 128;
  constexpr std::array<std::uint8_t, TRANSLATION_TABLE_SIZE> us_qwerty_translation = {
  0, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, 0x09, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
  0x0D, 0x11, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0x0E, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0x0F,
  '*', 0x13, ' ', 0x02,
  // function keys (F1-F10) here
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // Numlock and Scroll Lock:
  0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
  // three gaps
  0, 0, 0,
  // F11, F12
  0, 0};
   constexpr std::uint8_t SCAN_TABLE_1_FLAG_PRESSED = 0x80;
static std::uint8_t decode_keyboard_scancode(std::uint8_t scancode, const std::array<std::uint8_t, TRANSLATION_TABLE_SIZE>& translation_table,
                                      const std::uint8_t flag_pressed)
// std::uint8_t decode_keyboard_scancode(std::uint8_t scancode, std::span<uint8_t, TRANSLATION_TABLE_SIZE> &translation_table, const std::uint8_t
// flag_pressed)
{
    std::uint8_t decoded_keystroke = 0;
    // bool is_pressed = !(scancode & flag_pressed); // false means released
    //  klog::log("SCANCODE:: %2X, %2X", scancode, scancode & (~flag_pressed));
    //  klog::log("SIZE:: %d", table_size);
    std::uint8_t key_scancode = scancode & (~flag_pressed); // Stripped of press flag
    // if ((scancode & (~flag_pressed)) < ((translation_table_end - translation_table) / sizeof(translation_table[0])))
    (key_scancode < (translation_table.size())) && (decoded_keystroke = translation_table[key_scancode]);
    // klog::log("%2X, %d", decoded_keystroke, is_pressed);
    return decoded_keystroke;
}}

// void poll_keystroke(const std::uint8_t* translation_table, const std::uint8_t* translation_table_end)
void poll_keystroke()
{
    auto& translation_table = us_qwerty_translation;
    // auto translation_table =us_qwerty_translation;
    // auto translation_table_end = us_qwerty_translation + sizeof(us_qwerty_translation)/sizeof(us_qwerty_translation[0]);
    auto flag_pressed = SCAN_TABLE_1_FLAG_PRESSED;
    auto prev_msg = 0;
    while (true)
    {
        auto scancode = inb(ioports::PS2_KEYBOARD_DATA);
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
        bool is_pressed = !(scancode & flag_pressed);

        decoded_keystroke = decode_keyboard_scancode(scancode, translation_table, flag_pressed);
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
}
