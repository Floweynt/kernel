#include "tty.h"
#include <bits/user_implement.h>
#include <cstring>
static driver::tty_startup_driver* drv;

void driver::set_tty_startup_driver(driver::tty_startup_driver* d) { drv = d; }

void std::detail::putc(char ch) { drv->putc_handle_escape(ch); }

driver::tty_startup_driver::~tty_startup_driver() {}

void driver::tty_startup_driver::handle_color1()
{
    switch (*((uint64_t*)command_buffer[0]))
    {
    case 0x72:
    case 0x646572:
        color = tty::RED;
        break;
    case 0x67:
    case 0x6e65657267:
        color = tty::GREEN;
        break;
    case 0x62:
    case 0x65756c62:
        color = tty::BLUE;
        break;
    case 0x42:
    case 0x6b63616c62:
        color = tty::BLACK;
        break;
    case 0x57:
    case 0x6574696877:
        color = tty::WHITE;
        break;
    case 0x79:
    case 0x776f6c6c6579:
        color = tty::YELLOW;
        break;
    case 0x63:
    case 0x6E617963:
        color = tty::CYAN;
        break;
    case 0x47:
    case 0x79657267:
        color = tty::GREY;
        break;
    }
}

void driver::tty_startup_driver::putc_handle_escape(char c)
{
    if (is_command)
    {
        if (current_command == 0)
            current_command = c;
        else
        {
            if (is_text)
            {
                if (c == '\x1b')
                {
                    color = tty::WHITE;
                    is_command = false;
                    is_text = false;
                    return;
                }

                putc(c);
            }
            else if (c == ';')
            {
                if (std::islower(current_command))
                {
                    handler_type handler = handlers[current_command - 'a'][index];
                    if (handler)
                        (this->*handler)();
                }
                is_text = true;
            }
            else if (c == ',')
                index++;
            else
            {
                if (chindex < 32 && index < 8)
                    command_buffer[index][chindex++] = c;
            }
        }
    }
    else if (c == '\x1b')
    {
        is_command = true;
        current_command = 0;
        std::memset((void*)command_buffer, 0, sizeof(command_buffer));
        index = 0;
        chindex = 0;
    }
    else
        putc(c);
};
