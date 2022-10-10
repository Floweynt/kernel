// cSpell:ignore ubsan
#include <cstdint>
#include <klog/klog.h>

#if defined(COMPILER_CLANG)
#define NO_UBSAN [[clang::no_sanitize("undefined")]]
#elif defined(COMPILER_GCC)
#define NO_UBSAN [[gcc::no_sanitize("undefined")]]
#endif

#define C extern "C"

using value = std::uintptr_t;

// simple UBSAN types
class ubsan_type_descriptor
{
    std::uint16_t type_kind;
    std::uint16_t type_info;
    char type_name[1];

public:
    enum type
    {
        INTEGER = 0x0000,
        FLOAT = 0x0001,
        UNKNOWN = 0xffff
    };

    constexpr const char* get_type_name() const { return type_name; }
    constexpr type get_type() const { return static_cast<type>(type_kind); }
    constexpr bool is_integer() const { return get_type() == INTEGER; }
    constexpr bool is_signed_integer() const { return is_integer() && (type_info & 1); }
    constexpr bool is_unsigned_integer() const { return is_integer() && !(type_info & 1); }
    constexpr std::uint32_t get_integer_bit_width() const { return 1 << (type_info >> 1); }
    constexpr bool is_float() const { return get_type() == FLOAT; }
    constexpr std::uint32_t get_float_bit_width() const { return type_info; }
};

class ubsan_typed_value
{
    ubsan_type_descriptor& desc;
    value val;
    inline static constexpr unsigned BITS = sizeof(value) * 8;

public:
    constexpr ubsan_typed_value(ubsan_type_descriptor& desc, value val) : desc(desc), val(val) {}

    NO_UBSAN constexpr bool is_inline_int() const
    {
        const unsigned n = desc.get_integer_bit_width();
        return n <= BITS;
    }

    NO_UBSAN constexpr bool is_inline_float() const
    {
        const unsigned n = desc.get_float_bit_width();
        return n <= BITS;
    }

    NO_UBSAN std::intmax_t get_signed() const
    {
        if (is_inline_int())
        {
            const unsigned extra_bits = sizeof(std::intmax_t) * 8 - desc.get_integer_bit_width();
            return std::intmax_t(std::uintmax_t(val) << extra_bits) >> extra_bits;
        }

        if (desc.get_integer_bit_width() == 64)
            return *reinterpret_cast<std::int64_t*>(val);

        klog::panic("reached unreachable");
    }

    NO_UBSAN std::uintmax_t get_unsigned() const
    {
        if (is_inline_int())
            return val;
        if (desc.get_integer_bit_width() == 64)
            return *reinterpret_cast<std::uint64_t*>(val);
        __builtin_unreachable();
    }

    NO_UBSAN std::uintmax_t get_positive() const
    {
        if (desc.is_unsigned_integer())
            return get_unsigned();
        std::intmax_t Val = get_unsigned();
        return Val;
    }
};

struct ubsan_source_location
{
    const char* filename;
    std::uint32_t line;
    std::uint32_t column;
};

#define UBSAN_LOG_POS(name, loc) klog::log("ubsan: " name " @ %s:%d:%d", (loc).filename, (loc).line, (loc).column);

#define TYPENAME data->type->get_type_name()

// debugging
void __ubsan_hook_start() {}

// overflow
namespace ubsan
{
    struct ubsan_overflow
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* type;
    };

    NO_UBSAN [[noreturn]] void handle_math_overflow(ubsan_overflow* data, value lhs, value rhs, char op)
    {
        __ubsan_hook_start();
        ubsan_typed_value lhs_typed(*data->type, lhs);
        ubsan_typed_value rhs_typed(*data->type, rhs);
        UBSAN_LOG_POS("__ubsan_handle_math_overflow", data->loc);

        if (data->type->is_unsigned_integer())
            klog::log("ubsan: unsigned integer overflow when calculating %lu %c %lu", lhs_typed.get_unsigned(), op,
                      rhs_typed.get_unsigned());
        else
            klog::log("ubsan: signed integer overflow when calculating %ld %c %ld", lhs_typed.get_signed(), op,
                      rhs_typed.get_signed());

        klog::log("ubsan (note): type: %s", TYPENAME);
        klog::panic("__ubsan_handle_math_overflow");
    }

    C [[noreturn]] NO_UBSAN void __ubsan_handle_add_overflow(ubsan_overflow* data, value lhs, value rhs)
    {
        handle_math_overflow(data, lhs, rhs, '+');
    }

    C [[noreturn]] NO_UBSAN void __ubsan_handle_sub_overflow(ubsan_overflow* data, value lhs, value rhs)
    {

        handle_math_overflow(data, lhs, rhs, '/');
    }

    C [[noreturn]] NO_UBSAN void __ubsan_handle_mul_overflow(ubsan_overflow* data, value lhs, value rhs)
    {
        handle_math_overflow(data, lhs, rhs, '*');
    }

    C [[noreturn]] NO_UBSAN void __ubsan_handle_divrem_overflow(ubsan_overflow* data, value lhs, value rhs)
    {
        __ubsan_hook_start();
        ubsan_typed_value lhs_typed(*data->type, lhs);
        ubsan_typed_value rhs_typed(*data->type, rhs);

        UBSAN_LOG_POS("__ubsan_handle_divrem_overflow", data->loc);

        if (data->type->is_signed_integer() && rhs_typed.get_signed() == -1)
        {
            klog::log("ubsan: division of %ld by -1 cannot be represented\n"
                      "ubsan (note): type %s",
                      lhs_typed.get_signed(), TYPENAME);
        }
        else if (data->type->is_integer())
        {
            klog::log("ubsan: division by zero");
        }

        klog::panic("__ubsan_handle_divrem_overflow");
    }

    C [[noreturn]] NO_UBSAN void __ubsan_handle_negate_overflow(ubsan_overflow* data, value val)
    {
        __ubsan_hook_start();
        ubsan_typed_value val_typed(*data->type, val);

        UBSAN_LOG_POS("__ubsan_handle_negate_overflow", data->loc);

        if (data->type->is_signed_integer())
        {
            klog::log("ubsan: negation of %ld cannot be represented\n"
                      "ubsan (note): cast to an unsigned type to negate this value to itself",
                      val_typed.get_signed());
        }
        else
        {
            klog::log("ubsan: negation of %lu cannot be represented\n", val_typed.get_unsigned());
        }

        klog::log("ubsan (note): type %s", TYPENAME);

        klog::panic("__ubsan_handle_negate_overflow");
    }

    C [[noreturn]] NO_UBSAN void __ubsan_handle_pointer_overflow(ubsan_overflow* data, value base, value result)
    {
        __ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_pointer_overflow", data->loc);

        if (base == 0 && result == 0)
            klog::log("ubsan: applying zero offset to null pointer (why would you do that?)");
        else if (base == 0 && result != 0)
            klog::log("ubsan: applying non-zero offset %lu to null pointer", result);
        else if (base != 0 && result == 0)
            klog::log("ubsan: applying non-zero offset to non-null pointer %lu produced null pointer", base);
        else if ((std::intptr_t(base) >= 0) == (std::intptr_t(result) >= 0))
        {
            if (base > result)
                klog::log("ubsan: addition of unsigned offset to %lu overflowed to %lu", base, result);
            else
                klog::log("ubsan: subtraction of unsigned offset from %lu overflowed to %lu", base, result);
        }
        else
        {
            klog::log("ubsan: pointer index expression with base %lu overflowed to %lu", base, result);
        }

        klog::panic("__ubsan_handle_pointer_overflow");
    }
} // namespace ubsan

// shift OOB
namespace ubsan
{
    struct ubsan_shift_out_of_bounds
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* lhs_type;
        ubsan_type_descriptor* rhs_type;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_shift_out_of_bounds(ubsan_shift_out_of_bounds* data, value lhs, value rhs)
    {
        __ubsan_hook_start();
        ubsan_typed_value lhs_value(*data->lhs_type, lhs);
        ubsan_typed_value rhs_value(*data->rhs_type, rhs);
        UBSAN_LOG_POS("__ubsan_handle_shift_out_of_bounds", data->loc);

        if ((data->lhs_type->is_signed_integer() && lhs_value.get_signed() < 0) ||
            rhs_value.get_positive() >= data->lhs_type->get_integer_bit_width())
        {
            if (data->rhs_type->is_signed_integer() && rhs_value.get_signed() < 0)
                klog::log("ubsan: shift exponent %ld is negative", rhs_value.get_signed());
            else
                klog::log("ubsan: shift exponent %lu is too large for %u-bit type\n"
                          "ubsan (note): type %s",
                          rhs_value.get_unsigned(), data->lhs_type->get_integer_bit_width(),
                          data->lhs_type->get_type_name());
        }
        else
        {
            if (data->lhs_type->is_signed_integer() && lhs_value.get_signed() < 0)
                klog::log("left shift of negative value %ld", lhs_value.get_signed());
            else
                klog::log("left shift of %ld by %lu places cannot be represented\n"
                          "ubsan (note): type %s",
                          lhs_value.get_signed(), rhs_value.get_unsigned(), data->lhs_type->get_type_name());
        }

        klog::panic("__ubsan_handle_shift_out_of_bounds");
    }
} // namespace ubsan

namespace ubsan
{
    struct ubsan_invalid_value
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* type;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_load_invalid_value(ubsan_invalid_value* data, value val)
    {
        __ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_load_invalid_value", data->loc);

        klog::log("ubsan: load of value (pointer-or-integer) %lu, which is not a valid value for type %s", val,
                  data->type->get_type_name());

        klog::panic("__ubsan_handle_load_invalid_value");
    }
} // namespace ubsan

namespace ubsan
{
    struct ubsan_oob
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* type;
        ubsan_type_descriptor* index;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_out_of_bounds(ubsan_oob* data, value val)
    {
        __ubsan_hook_start();
        ubsan_typed_value index(*data->index, val);
        UBSAN_LOG_POS("__ubsan_handle_out_of_bounds", data->loc);

        klog::log("ubsan: index %lu out of bounds for type %s", index.get_unsigned(), data->index->get_type_name());

        klog::panic("__ubsan_handle_out_of_bounds");
    }
} // namespace ubsan

inline constexpr const char* const type_check_kinds[] = {
    "load of",
    "store to",
    "reference binding to",
    "member access within",
    "member call on",
    "constructor call on",
    "downcast of",
    "downcast of",
    "upcast of",
    "cast to virtual base of",
    "_Nonnull binding to",
    "dynamic operation on",
};

namespace ubsan
{
    struct ubsan_type_mismatch_v1
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* type;
        unsigned char log_alignment;
        unsigned char type_check_kind;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_type_mismatch_v1(ubsan_type_mismatch_v1* data, std::uintptr_t ptr)
    {
        __ubsan_hook_start();

        std::uintptr_t alignment = (std::uintptr_t)1 << data->log_alignment;
        const ubsan_source_location* location = &data->loc;

        if (location->filename)
        {
            UBSAN_LOG_POS("__ubsan_handle_type_mismatch_v1", data->loc);
        }
        else
            klog::log("__ubsan_handle_type_mismatch_v1 @ undeduced source location");

        if (!ptr)
        {
            klog::log("ubsan: %s null pointer of type %s", type_check_kinds[data->type_check_kind],
                      data->type->get_type_name());
        }
        else if (ptr & (alignment - 1))
        {
            klog::log("ubsan: %s misaligned address %lu for type %s\n"
                      "ubsan (note): requires %lu byte alignment",
                      type_check_kinds[data->type_check_kind], ptr, data->type->get_type_name(), alignment);
        }
        else
        {
            klog::log("ubsan: %s address %lu with insufficient space "
                      "for an object of type %s",
                      type_check_kinds[data->type_check_kind], ptr, data->type->get_type_name());
        }

        klog::panic("__ubsan_handle_type_mismatch_v1");
    }
} // namespace ubsan

namespace ubsan
{
    struct ubsan_vla_bound
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* type;
    };

    NO_UBSAN C void __ubsan_handle_nonnull_return_v1(ubsan_vla_bound* data, value val)
    {
        __ubsan_hook_start();
        ubsan_typed_value value(*data->type, val);
        UBSAN_LOG_POS("__ubsan_handle_nonnull_return_v1", data->loc);

        if (data->type->is_signed_integer())
            klog::log("ubsan: variable length array bound evaluates to non-positive value %ld", value.get_signed());
        if (data->type->is_unsigned_integer())
            klog::log("ubsan: variable length array bound evaluates to non-positive value %lu", value.get_unsigned());
        klog::panic("__ubsan_handle_nonnull_return_v1");
    }
} // namespace ubsan

namespace ubsan
{
    struct ubsan_nonnull_arg
    {
        ubsan_source_location loc;
        ubsan_source_location attr_loc;
        int arg_index;
    };

    NO_UBSAN C void __ubsan_handle_nonnull_arg(ubsan_nonnull_arg* data)
    {
        __ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_nonnull_return_v1", data->loc);

        klog::log("ubsan: null pointer passed as argument %u, which is declared to "
                  "never be null",
                  data->arg_index);

        if (!data->attr_loc.filename)
            klog::log("ubsan (note): nonnull attribute specified here %s:%d,%d", data->attr_loc.filename,
                      data->attr_loc.line, data->attr_loc.column);

        klog::panic("__ubsan_handle_nonnull_return_v1");
    }

} // namespace ubsan

namespace ubsan
{
    struct ubsan_unreachable
    {
        ubsan_source_location loc;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_builtin_unreachable(ubsan_unreachable* data)
    {
        __ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_builtin_unreachable", data->loc);
        klog::log("ubsan: reached code marked as unreachable");
        klog::panic("__ubsan_handle_builtin_unreachable");
    }
} // namespace ubsan
struct ubsan_function_type_mismatch
{
    ubsan_source_location loc;
    ubsan_type_descriptor* type;
};

namespace ubsan
{

    struct ubsan_invalid_builtin
    {
        struct ubsan_source_location loc;
        unsigned char kind;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_invalid_builtin(ubsan_invalid_builtin* data)
    {
        __ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_invalid_builtin", data->loc);
        klog::log("ubsan: passing zero to %s, which is not a valid argument", (data->kind == 0) ? "ctz()" : "clz()");
        klog::panic("__ubsan_handle_invalid_builtin");
    }
} // namespace ubsan

namespace ubsan
{
    struct ubsan_alignment_assumption
    {
        ubsan_source_location loc;
        ubsan_source_location assume_loc;
        ubsan_type_descriptor* type;
    };

    C [[noreturn]] NO_UBSAN void __ubsan_handle_alignment_assumption(ubsan_alignment_assumption* data, value ptr,
                                                                     value align, value off)
    {
        __ubsan_hook_start();

        std::uintptr_t real_ptr = ptr - off;
        std::uintptr_t real_align = std::uintptr_t(1) << __builtin_ctzl(real_ptr);

        std::uintptr_t align_mask = align - 1;
        std::uintptr_t misalign_off = real_ptr & align_mask;

        UBSAN_LOG_POS("__ubsan_handle_alignment_assumption", data->loc);

        if (!off)
        {
            klog::log("ubsan: assumption of %lu byte alignment for pointer of type %s failed", align,
                      data->type->get_type_name());
        }
        else
        {
            klog::log("assumption of %lu yte alignment (with offset of %lu byte) for pointer "
                      "of type %s failed",
                      align, off, data->type->get_type_name());
        }

        if (!data->assume_loc.filename)
            klog::log("ubsan (note): alignment assumption specified here %s:%d,%d", data->assume_loc.filename,
                      data->assume_loc.line, data->assume_loc.column);

        klog::log("%saddress is %lu aligned, misalignment offset is %lu bytes", (off ? "offset " : ""), real_align,
                  misalign_off);

        klog::panic("__ubsan_handle_alignment_assumption");
    }

} // namespace ubsan
