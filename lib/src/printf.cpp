#include <printf.h>

namespace std
{
    using _print_cb = void (*)(char ch, void* data);

    namespace detail::printf
    {
        namespace
        {
            enum status
            {
                ZEROPAD = 1 << 0,
                LEFT = 1 << 1,
                PLUS = 1 << 2,
                SPACE = 1 << 3,
                HASH = 1 << 4,
                UPPERCASE = 1 << 5,
            };

            enum length
            {
                NORMAL,
                CHAR,
                SHORT,
                LONG,
                LLONG,
                INTMAX,
                SIZE,
                PTRDIFF,
            };

            template <typename T>
            constexpr size_t floor_log(unsigned base, T val)
            {
                if (val < 0)
                    val = -val;
                size_t ret = 0;
                while ((val /= base))
                    ret++;
                return ret;
            }

            // guesses the printed length of an integer
            template <typename T>
            constexpr size_t guess_len(unsigned b, T val, unsigned flag, unsigned precision)
            {
                size_t ret = max(floor_log(b, val) + 1, (size_t)precision);

                if constexpr (is_signed_v<T>)
                {
                    if (val < 0)
                        ret++;
                    else if (flag & PLUS || flag & SPACE)
                        ret++;
                }

                return ret == 0 ? 1 : ret;
            }

            // obtains the amount of '0' to print
            template <typename T>
            constexpr size_t precision_pad_chars(unsigned b, T val, unsigned precision)
            {
                auto len = floor_log(b, val) + 1;
                return precision > len ? precision - len : 0;
            }

            struct printf_command
            {
                unsigned precision = 0;
                unsigned width = 0;
                int flags = 0;
                char padchar = ' ';
            };

            void handle_special(va_list fmtargs, const char*& format, printf_command& cmd, auto print)
            {
                size_t width_len;
                const char* str;
                char buf[sizeof(void*) * 2] = {};
                std::memset(buf, '0', sizeof(buf));
                const char* int2char = (isupper(*format) || (cmd.flags & UPPERCASE)) ? "0123456789ABCDEF" : "0123456789abcdef";
                bool b;
                switch (*format)
                {
                case 'c':
                    width_len = 1;
                    break;
                case 's':
                    width_len = strlen(str = va_arg(fmtargs, const char*));
                    break;
                case 'p':
                    width_len = sizeof(void*) * 2;
                    break;
                case 'B':
                    width_len = (b = va_arg(fmtargs, int)) ? 4 : 5;
                    break;
                }

                width_len = cmd.width > width_len ? cmd.width - width_len : 0;

                if (!(cmd.flags & LEFT))
                    for (size_t i = 0; i < width_len; i++)
                        print(cmd.padchar);

                switch (*format)
                {
                case 'c':
                    print(va_arg(fmtargs, int));
                    break;
                case 's': {
                    for (unsigned i = 0; (i < cmd.precision || cmd.precision == 0) && *str; i++)
                    {
                        print(*str++);
                    }
                    break;
                }
                case 'p': {
                    uint64_t v = (uint64_t)va_arg(fmtargs, void*);
                    size_t index = 0;
                    while (v)
                    {
                        buf[index++] = int2char[v % 16];
                        v /= 16;
                    }

                    for (int i = sizeof(void*) * 2 - 1; i >= 0; i--)
                        print(buf[i]);
                    break;
                }
                case 'B':
                    if (b)
                    {
                        print('t');
                        print('r');
                        print('u');
                        print('e');
                    }
                    else
                    {
                        print('f');
                        print('a');
                        print('l');
                        print('s');
                        print('e');
                    }
                }

                if (cmd.flags & LEFT)
                    for (size_t i = 0; i < width_len; i++)
                        print(cmd.padchar);
            }

            // prints an properly formatted integer
            template <typename s, typename u, typename p>
            void format_int(va_list fmtargs, const printf_command& cmd, const char* format, size_t pc, auto print)
            {
                char tmp[32] = {0};
                const char* int2char = (isupper(*format) || (cmd.flags & UPPERCASE)) ? "0123456789ABCDEF" : "0123456789abcdef";
                size_t width_len = 0;
                size_t precision_len = 0;
                bool sign = false;
                bool is_signed = false;

                auto itoa = [&](unsigned radix, auto val) {
                    if (val == 0)
                    {
                        tmp[0] = '0';
                        return;
                    }
                    if constexpr (is_signed_v<decltype(val)>)
                        if (val < 0)
                            val = -val;
                    size_t index = 0;
                    while (val)
                    {
                        tmp[index++] = int2char[val % radix];
                        val /= radix;
                    }
                };

                using _s = std::conditional_t<(sizeof(s) > sizeof(int)), s, int>;
                using _u = std::conditional_t<(sizeof(u) > sizeof(unsigned int)), u, unsigned int>;

                switch (*format)
                {
                case 'd':
                case 'i': {
                    s val = va_arg(fmtargs, _s);
                    itoa(10, val);
                    if (val < 0)
                        sign = true;
                    is_signed = true;
                    width_len = guess_len(10, val, cmd.flags, cmd.precision);
                    precision_len = precision_pad_chars(10, val, cmd.precision);
                    break;
                }
                case 'u': {
                    u val = va_arg(fmtargs, _u);
                    itoa(10, val);
                    width_len = guess_len(10, val, cmd.flags, cmd.precision);
                    precision_len = precision_pad_chars(10, val, cmd.precision);
                    break;
                }
                case 'o': {
                    u val = va_arg(fmtargs, _u);
                    itoa(8, val);
                    width_len = guess_len(8, val, cmd.flags, cmd.precision);
                    precision_len = precision_pad_chars(8, val, cmd.precision);
                    break;
                }
                case 'x':
                case 'X': {
                    u val = va_arg(fmtargs, _u);
                    itoa(16, val);
                    width_len = guess_len(16, val, cmd.flags, cmd.precision);
                    precision_len = precision_pad_chars(16, val, cmd.precision);
                    break;
                }
                case 'n':
                    *va_arg(fmtargs, p) = pc;
                    return;
                default:
                    detail::errors::__printf_undefined_specifier_for_length();
                }

                width_len = cmd.width > width_len ? cmd.width - width_len : 0;

                if (!(cmd.flags & LEFT))
                    for (size_t i = 0; i < width_len; i++)
                        print(cmd.padchar);

                if (sign)
                    print('-');
                else if (is_signed)
                {
                    if (cmd.flags & SPACE)
                        print(' ');
                    else if (cmd.flags & PLUS)
                        print('+');
                }

                for (size_t i = 0; i < precision_len; i++)
                    print('0');

                for (int i = 15; i >= 0; i--)
                {
                    if (tmp[i])
                        print(tmp[i]);
                }

                if (cmd.flags & LEFT)
                    for (size_t i = 0; i < width_len; i++)
                        print(cmd.padchar);
            }
        } // namespace

        int do_printf(_print_cb printer, void* data, const char* format, va_list fmtargs)
        {
            using namespace detail::printf;
            size_t printed_characters = 0;

            auto atoi_inc_str = [&]() mutable {
                unsigned int i = 0;
                while (isdigit(*format))
                    i = i * 10U + (unsigned int)(*(format++) - '0');
                return i;
            };

            auto print = [&](char ch) mutable {
                printer(ch, data);
                printed_characters++;
            };

            while (*format)
            {
                printf_command cmd;
                length len;

                if (*format != '%')
                {
                    print(*format);
                    format++;
                    continue;
                }
                else
                    format++;

                switch (*format)
                {
                case '0':
                    cmd.padchar = '0';
                    break;
                case '-':
                    cmd.flags |= LEFT;
                    break;
                case '+':
                    cmd.flags |= PLUS;
                    break;
                case '#':
                    cmd.flags |= HASH;
                    break;
                case ' ':
                    cmd.flags |= SPACE;
                    break;
                }

                if (cmd.flags)
                    format++;

                if (isdigit(*format))
                    cmd.width = atoi_inc_str();

                else if (*format == '*')
                {
                    auto w = va_arg(fmtargs, int);
                    if (w < 0)
                    {
                        cmd.flags |= LEFT;
                        w = -w;
                    }
                    else
                        cmd.width = (unsigned int)w;

                    cmd.width = w;
                    format++;
                }

                if (*format == '.')
                {
                    format++;
                    if (isdigit(*format))
                        cmd.precision = atoi_inc_str();
                    else if (*format == '*')
                    {
                        auto prec = va_arg(fmtargs, int);
                        cmd.precision = prec > 0 ? prec : 0;
                        format++;
                    }
                }

                switch (*format)
                {
                case 'l':
                    len = LONG;
                    format++;
                    if (*format == 'l')
                    {
                        len = LLONG;
                        format++;
                    }
                    break;
                case 'h':
                    len = SHORT;
                    format++;
                    if (*format == 'h')
                    {
                        len = CHAR;
                        format++;
                    }
                    break;
                case 't':
                    len = PTRDIFF;
                    format++;
                    break;
                case 'j':
                    len = INTMAX;
                    format++;
                    break;
                case 'z':
                    len = SIZE;
                    format++;
                    break;
                default:
                    len = NORMAL;
                    break;
                }

                if (*format == 'c' || *format == 's' || *format == 'p' || *format == 'B')
                {
                    if (len != NORMAL)
                        detail::errors::__printf_undefined_specifier_for_length();
                    handle_special(fmtargs, format, cmd, print);
                    format++;
                    continue;
                }

                switch (len)
                {
                case NORMAL:
                    format_int<int, unsigned, int*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case CHAR:
                    format_int<signed char, unsigned char, char*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case SHORT:
                    format_int<short, unsigned short, short*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case LONG:
                    format_int<long, unsigned long, long*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case LLONG:
                    format_int<long long, unsigned long long, long long*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case INTMAX:
                    format_int<intmax_t, uintmax_t, intmax_t*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case PTRDIFF:
                    format_int<ptrdiff_t, ptrdiff_t, ptrdiff_t*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                case SIZE:
                    format_int<size_t, size_t, size_t*>(fmtargs, cmd, format, printed_characters, print);
                    break;
                }

                format++;
            }
            printer('\0', data);
            return printed_characters;
        }
    } // namespace detail::printf
} // namespace std
