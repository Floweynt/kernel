#include "serial.h"

namespace driver
{
    static int is_transmit_empty()
    {
        return inb(serial_tty_driver::PORT_COM1 + 5) & 0x20;
    }

    static void write_serial(char a)
    {
        while (is_transmit_empty() == 0);

        outb(serial_tty_driver::PORT_COM1, a);
    }

    void serial_tty_driver::init()
    {
        outb(PORT_COM1 + 1, 0x00);    // Disable all interrupts
        outb(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        outb(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        outb(PORT_COM1 + 1, 0x00);    //                  (hi byte)
        outb(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
        outb(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        outb(PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        outb(PORT_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
        outb(PORT_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

        if(inb(PORT_COM1 + 0) != 0xAE)
            return;

        outb(PORT_COM1 + 4, 0x0F);
        puts("Console driver initalized!\n");
    }

    void serial_tty_driver::puts(const char* c)
    {
        while(*c)
        {
            write_serial(*c);
            c++;
        }
    }
}
