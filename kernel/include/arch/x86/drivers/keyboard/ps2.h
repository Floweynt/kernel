#include <asm/asm_cpp.h>
#include <klog/klog.h>
std::pair<uint8_t, bool> decode_keyboard_scancode(uint8_t scancode, uint8_t translation_table[], constexpr uint8_t FLAG_RELEASED)
{
  uint8_t decoded_keypress = 0;
  bool is_pressed = true; // false means released
  
  if (scancode &(!FLAG_RELEASED)< (sizeof(translation_table) / sizeof(translation_table[0])))
  {
      decoded_keypress = translation_table[scancode];
  }
    is_pressed =scancode&FLAG_RELEASED;
  return std::make_pair(scancode,is_pressed);
}
