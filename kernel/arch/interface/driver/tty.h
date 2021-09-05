class tty_startup_driver
{
public:
    virtual void putc(char c) = 0;
    virtual ~tty_startup_driver() = default;
};
