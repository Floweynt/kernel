#include "asm/asm_cpp.h"
#include "interface/driver/tty.h"

namespace driver
{
    class serial_tty_driver : public tty_startup_driver
    {
    public:
        static constexpr auto PORT_COM1 = 0x3f8;
        inline serial_tty_driver() { }
        void init();
        void puts(const char* c);
    };
}
