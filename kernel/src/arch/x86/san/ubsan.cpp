// cSpell:ignore ubsan
#include "sync/spinlock.h"
#include <cstdint>
#include <klog/klog.h>
#include <misc/cast.h>
#include <tty/tty.h>

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

    [[nodiscard]] constexpr auto get_type_name() const -> const char* { return decay_arr(type_name); }
    [[nodiscard]] constexpr auto get_type() const -> type { return static_cast<type>(type_kind); }
    [[nodiscard]] constexpr auto is_integer() const -> bool { return get_type() == INTEGER; }
    [[nodiscard]] constexpr auto is_signed_integer() const -> bool { return is_integer() && ((type_info & 1) != 0); }
    [[nodiscard]] constexpr auto is_unsigned_integer() const -> bool { return is_integer() && ((type_info & 1) == 0); }
    [[nodiscard]] constexpr auto get_integer_bit_width() const -> std::uint32_t { return 1 << (type_info >> 1); }
    [[nodiscard]] constexpr auto is_float() const -> bool { return get_type() == FLOAT; }
    [[nodiscard]] constexpr auto get_float_bit_width() const -> std::uint32_t { return type_info; }
};

class ubsan_typed_value
{
    ubsan_type_descriptor& desc;
    value val;
    inline static constexpr unsigned BITS = sizeof(value) * 8;

public:
    constexpr ubsan_typed_value(ubsan_type_descriptor& desc, value val) : desc(desc), val(val) {}

    NO_UBSAN [[nodiscard]] constexpr auto is_inline_int() const -> bool
    {
        const unsigned width = desc.get_integer_bit_width();
        return width <= BITS;
    }

    NO_UBSAN [[nodiscard]] constexpr auto is_inline_float() const -> bool
    {
        const unsigned width = desc.get_float_bit_width();
        return width <= BITS;
    }

    NO_UBSAN [[nodiscard]] auto get_signed() const -> std::intmax_t
    {
        if (is_inline_int())
        {
            const unsigned extra_bits = sizeof(std::intmax_t) * 8 - desc.get_integer_bit_width();
            return std::intmax_t(std::uintmax_t(val) << extra_bits) >> extra_bits;
        }

        if (desc.get_integer_bit_width() == 64)
        {
            return *as_ptr<std::int64_t>(val);
        }

        klog::panic("reached unreachable");
    }

    NO_UBSAN [[nodiscard]] auto get_unsigned() const -> std::uintmax_t
    {
        if (is_inline_int())
        {
            return val;
        }
        if (desc.get_integer_bit_width() == 64)
        {
            return *as_ptr<std::uint64_t>(val);
        }
        __builtin_unreachable();
    }

    NO_UBSAN [[nodiscard]] auto get_positive() const -> std::uintmax_t
    {
        if (desc.is_unsigned_integer())
        {
            return get_unsigned();
        }
        return get_unsigned();
    }
};

struct ubsan_source_location
{
    const char* filename;
    std::uint32_t line;
    std::uint32_t column;
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UBSAN_LOG_POS(name, loc) klog::log(RED("ubsan") ": " name " @ %s:%d:%d", (loc).filename, (loc).line, (loc).column);

#define TYPENAME data->type->get_type_name()

// debugging
void ubsan_hook_start()
{
    static lock::spinlock lock;
    lock.lock();
    klog::log("====================== " RED("ubsan") " ======================");
    klog::log("a runtime kernel error has occurred that cannot be recovered from due to undefined behaviour");
    klog::log("now why did I mess up?");
    // dont release since UBSAN is always fatal and should hang the system
}

// overflow
namespace ubsan
{
    struct ubsan_overflow
    {
        ubsan_source_location loc;
        ubsan_type_descriptor* type;
    };

    NO_UBSAN [[noreturn]] void handle_math_overflow(ubsan_overflow* data, value lhs, value rhs, char oper)
    {
        ubsan_hook_start();
        ubsan_typed_value lhs_typed(*data->type, lhs);
        ubsan_typed_value rhs_typed(*data->type, rhs);
        UBSAN_LOG_POS("__ubsan_handle_math_overflow", data->loc);

        if (data->type->is_unsigned_integer())
        {
            klog::log(RED("ubsan") ": unsigned integer overflow when calculating %lu %c %lu", lhs_typed.get_unsigned(), oper,
                      rhs_typed.get_unsigned());
        }
        else
        {
            klog::log(RED("ubsan") ": signed integer overflow when calculating %ld %c %ld", lhs_typed.get_signed(), oper, rhs_typed.get_signed());
        }

        klog::log(RED("ubsan") " (note): type: %s", TYPENAME);
        klog::panic("__ubsan_handle_math_overflow");
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_add_overflow(ubsan_overflow* data, value lhs, value rhs)
    {
        handle_math_overflow(data, lhs, rhs, '+');
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_sub_overflow(ubsan_overflow* data, value lhs, value rhs)
    {

        handle_math_overflow(data, lhs, rhs, '/');
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_mul_overflow(ubsan_overflow* data, value lhs, value rhs)
    {
        handle_math_overflow(data, lhs, rhs, '*');
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_divrem_overflow(ubsan_overflow* data, value lhs, value rhs)
    {
        ubsan_hook_start();
        ubsan_typed_value lhs_typed(*data->type, lhs);
        ubsan_typed_value rhs_typed(*data->type, rhs);

        UBSAN_LOG_POS("__ubsan_handle_divrem_overflow", data->loc);

        if (data->type->is_signed_integer() && rhs_typed.get_signed() == -1)
        {
            klog::log(RED("ubsan") ": division of %ld by -1 cannot be represented", lhs_typed.get_signed());
            klog::log(RED("ubsan") " (note): type %s", TYPENAME);
        }
        else if (data->type->is_integer())
        {
            klog::log(RED("ubsan") ": division by zero");
        }

        klog::panic("__ubsan_handle_divrem_overflow");
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_negate_overflow(ubsan_overflow* data, value val)
    {
        ubsan_hook_start();
        ubsan_typed_value val_typed(*data->type, val);

        UBSAN_LOG_POS("__ubsan_handle_negate_overflow", data->loc);

        if (data->type->is_signed_integer())
        {
            klog::log(RED("ubsan") ": negation of %ld cannot be represented", val_typed.get_signed());
            klog::log(RED("ubsan") " (note): cast to an unsigned type to negate this value to itself");
        }
        else
        {
            klog::log(RED("ubsan") ": negation of %lu cannot be represented", val_typed.get_unsigned());
        }

        klog::log(RED("ubsan") " (note): type %s", TYPENAME);

        klog::panic("__ubsan_handle_negate_overflow");
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_pointer_overflow(ubsan_overflow* data, value base, value result)
    {
        ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_pointer_overflow", data->loc);

        if (base == 0 && result == 0)
        {
            klog::log(RED("ubsan") ": applying zero offset to null pointer (why would you do that?)");
        }
        else if (base == 0 && result != 0)
        {
            klog::log(RED("ubsan") ": applying non-zero offset %lu to null pointer", result);
        }
        else if (base != 0 && result == 0)
        {
            klog::log(RED("ubsan") ": applying non-zero offset to non-null pointer %016lx produced null pointer", base);
        }
        else if ((std::intptr_t(base) >= 0) == (std::intptr_t(result) >= 0))
        {
            if (base > result)
            {
                klog::log(RED("ubsan") ": addition of unsigned offset to 0x%016lx overflowed to 0x%016lx", base, result);
            }
            else
            {
                klog::log(RED("ubsan") ": subtraction of unsigned offset from 0x%016lx overflowed to 0x%016lx", base, result);
            }
        }
        else
        {
            klog::log(RED("ubsan") ": pointer index expression with base 0x%016lx overflowed to 0x%016lx", base, result);
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_shift_out_of_bounds(ubsan_shift_out_of_bounds* data, value lhs, value rhs)
    {
        ubsan_hook_start();
        ubsan_typed_value lhs_value(*data->lhs_type, lhs);
        ubsan_typed_value rhs_value(*data->rhs_type, rhs);
        UBSAN_LOG_POS("__ubsan_handle_shift_out_of_bounds", data->loc);

        if ((data->lhs_type->is_signed_integer() && lhs_value.get_signed() < 0) ||
            rhs_value.get_positive() >= data->lhs_type->get_integer_bit_width())
        {
            if (data->rhs_type->is_signed_integer() && rhs_value.get_signed() < 0)
            {
                klog::log(RED("ubsan") ": shift exponent %ld is negative", rhs_value.get_signed());
            }
            else
            {
                klog::log(RED("ubsan") ": shift exponent %lu is too large for %u-bit type", rhs_value.get_unsigned(),
                          data->lhs_type->get_integer_bit_width());
            }
        }
        else
        {
            if (data->lhs_type->is_signed_integer() && lhs_value.get_signed() < 0)
            {
                klog::log(RED("ubsan") ": left shift of negative value %ld", lhs_value.get_signed());
            }
            else
            {
                klog::log(RED("ubsan") ": left shift of %ld by %lu places cannot be represented", lhs_value.get_signed(), rhs_value.get_unsigned());
            }
        }

        klog::log(RED("ubsan") " (note): type %s", data->lhs_type->get_type_name());
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_load_invalid_value(ubsan_invalid_value* data, value val)
    {
        ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_load_invalid_value", data->loc);

        klog::log(RED("ubsan") ": load of value (pointer-or-integer) 0x%016lx, which is not a valid value for type %s", val,
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_out_of_bounds(ubsan_oob* data, value val)
    {
        ubsan_hook_start();
        ubsan_typed_value index(*data->index, val);
        UBSAN_LOG_POS("__ubsan_handle_out_of_bounds", data->loc);

        klog::log(RED("ubsan") ": index %lu out of bounds for type %s", index.get_unsigned(), data->index->get_type_name());

        klog::panic("__ubsan_handle_out_of_bounds");
    }
} // namespace ubsan

inline constexpr std::array type_check_kinds = {
    "load of",     "store to",  "reference binding to",    "member access within", "member call on",       "constructor call on", "downcast of",
    "downcast of", "upcast of", "cast to virtual base of", "_Nonnull binding to",  "dynamic operation on",
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_type_mismatch_v1(ubsan_type_mismatch_v1* data, std::uintptr_t ptr)
    {
        ubsan_hook_start();

        std::uintptr_t alignment = (std::uintptr_t)1 << data->log_alignment;
        const ubsan_source_location* location = &data->loc;

        if (location->filename != nullptr)
        {
            UBSAN_LOG_POS("__ubsan_handle_type_mismatch_v1", data->loc);
        }
        else
        {
            klog::log("__ubsan_handle_type_mismatch_v1 @ undeduced source location");
        }

        if (!ptr)
        {
            klog::log(RED("ubsan") ": %s null pointer of type %s", type_check_kinds.at(data->type_check_kind), data->type->get_type_name());
        }
        else if (ptr & (alignment - 1))
        {
            klog::log(RED("ubsan") ": %s misaligned address 0x%016lx for type %s", type_check_kinds.at(data->type_check_kind), ptr,
                      data->type->get_type_name());
            klog::log(RED("ubsan") " (note): requires 0x%lx byte alignment", alignment);
        }
        else
        {
            klog::log(RED("ubsan") ": %s address 0x%016lx with insufficient space for an object of type %s",
                      type_check_kinds.at(data->type_check_kind), ptr, data->type->get_type_name());
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C NO_UBSAN void __ubsan_handle_nonnull_return_v1(ubsan_vla_bound* data, value val)
    {
        ubsan_hook_start();
        ubsan_typed_value value(*data->type, val);
        UBSAN_LOG_POS("__ubsan_handle_nonnull_return_v1", data->loc);

        if (data->type->is_signed_integer())
        {
            klog::log(RED("ubsan") ": variable length array bound evaluates to non-positive value %ld", value.get_signed());
        }
        else
        {
            klog::log(RED("ubsan") ": variable length array bound evaluates to non-positive value %lu", value.get_unsigned());
        }
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C NO_UBSAN void __ubsan_handle_nonnull_arg(ubsan_nonnull_arg* data)
    {
        ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_nonnull_return_v1", data->loc);

        klog::log(RED("ubsan") ": null pointer passed as argument %u, which is declared to never be null", data->arg_index);

        if (data->attr_loc.filename == nullptr)
        {
            klog::log(RED("ubsan") " (note): nonnull attribute specified here %s:%d,%d", data->attr_loc.filename, data->attr_loc.line,
                      data->attr_loc.column);
        }

        klog::panic("__ubsan_handle_nonnull_return_v1");
    }

} // namespace ubsan

namespace ubsan
{
    struct ubsan_unreachable
    {
        ubsan_source_location loc;
    };

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_builtin_unreachable(ubsan_unreachable* data)
    {
        ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_builtin_unreachable", data->loc);
        klog::log(RED("ubsan") ": reached code marked as unreachable");
        klog::panic("__ubsan_handle_builtin_unreachable");
    }

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_missing_return(ubsan_unreachable* data)
    {
        ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_missing_return", data->loc);
        klog::log(RED("ubsan") ": reached end-of-function without return");
        klog::panic("__ubsan_handle_missing_return");
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_invalid_builtin(ubsan_invalid_builtin* data)
    {
        ubsan_hook_start();
        UBSAN_LOG_POS("__ubsan_handle_invalid_builtin", data->loc);
        klog::log(RED("ubsan") ": passing zero to %s, which is not a valid argument", (data->kind == 0) ? "ctz()" : "clz()");
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

    // NOLINTNEXTLINE(cert-dcl51-cpp,cert-dcl37-c,bugprone-reserved-identifier)
    C [[noreturn]] NO_UBSAN void __ubsan_handle_alignment_assumption(ubsan_alignment_assumption* data, value ptr, value align, value off)
    {
        ubsan_hook_start();

        std::uintptr_t real_ptr = ptr - off;
        std::uintptr_t real_align = std::uintptr_t(1) << __builtin_ctzl(real_ptr);

        std::uintptr_t align_mask = align - 1;
        std::uintptr_t misalign_off = real_ptr & align_mask;

        UBSAN_LOG_POS("__ubsan_handle_alignment_assumption", data->loc);

        if (!off)
        {
            klog::log(RED("ubsan") ": assumption of 0x%lx byte alignment for pointer of type %s failed", align, data->type->get_type_name());
        }
        else
        {
            klog::log("assumption of 0x%lx byte alignment (with offset of 0x%lx byte) for pointer of type %s failed", align, off,
                      data->type->get_type_name());
        }

        if (data->assume_loc.filename != nullptr)
        {
            klog::log(RED("ubsan") " (note): alignment assumption specified here %s:%d:%d", data->assume_loc.filename, data->assume_loc.line,
                      data->assume_loc.column);
        }

        klog::log("%saddress is 0x%016lx aligned, misalignment offset is 0x%lx bytes", (off ? "offset " : ""), real_align, misalign_off);

        klog::panic("__ubsan_handle_alignment_assumption");
    }
} // namespace ubsan
