#include <klog/klog.h>

struct ubsan_source_location
{
    const char* file;
    std::uint32_t line;
    std::uint32_t column;
};

struct ubsan_type_descriptor
{
    std::uint16_t kind;
    std::uint16_t info;
    char name[];
};

struct ubsan_overflow_data
{
    ubsan_source_location location;
    ubsan_type_descriptor* type;
};

struct ubsan_shift_out_of_bounds_data
{
    ubsan_source_location location;
    ubsan_type_descriptor* left_type;
    ubsan_type_descriptor* right_type;
};

struct ubsan_invalid_value_data
{
    ubsan_source_location location;
    ubsan_type_descriptor* type;
};

struct ubsan_array_out_of_bounds_data
{
    ubsan_source_location location;
    ubsan_type_descriptor* array_type;
    ubsan_type_descriptor* index_type;
};

struct ubsan_type_mismatch_v1_data
{
    ubsan_source_location location;
    ubsan_type_descriptor* type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

struct ubsan_negative_vla_data
{
    ubsan_source_location location;
    ubsan_type_descriptor* type;
};

struct ubsan_nonnull_return_data
{
    ubsan_source_location location;
};

struct ubsan_nonnull_arg_data
{
    ubsan_source_location location;
};

struct ubsan_unreachable_data
{
    ubsan_source_location location;
};

struct ubsan_invalid_builtin_data
{
    ubsan_source_location location;
    unsigned char kind;
};

static ubsan_source_location UB_UNK{"unimplemented", 0, 0};

static void ubsan_fail(const char* message, const ubsan_source_location& loc)
{
    klog::log("ubsan: %s at file %s:%d:%d\n", message, loc.file, loc.line, loc.column);
    klog::panic("ubsan");
}

#define location_or_unk (data && data->location.line != 0) ? data->location : UB_UNK

#if defined(COMPILER_CLANG) 
#define NO_UBSAN [[clang::no_sanitize("undefined")]]
#elif defined(COMPILER_GCC)
#define NO_UBSAN [[gnu::no_sanitize("undefined")]]
#endif

extern "C"
{
    NO_UBSAN void __ubsan_handle_add_overflow_minimal(ubsan_overflow_data* data) 
    { 
        ubsan_fail("addition overflow", location_or_unk); 
    }

    NO_UBSAN void __ubsan_handle_sub_overflow_minimal(ubsan_overflow_data* data)
    {
        ubsan_fail("subtraction overflow", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_mul_overflow_minimal(ubsan_overflow_data* data)
    {
        ubsan_fail("multiplication overflow", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_divrem_overflow_minimal(ubsan_overflow_data* data)
    {
        ubsan_fail("division overflow", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_negate_overflow_minimal(ubsan_overflow_data* data)
    {
        ubsan_fail("negation overflow", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_pointer_overflow_minimal(ubsan_overflow_data* data)
    {
        ubsan_fail("pointer overflow", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_shift_out_of_bounds_minimal(ubsan_shift_out_of_bounds_data* data)
    {
        ubsan_fail("shift out of bounds", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_load_invalid_value_minimal(ubsan_invalid_value_data* data)
    {
        ubsan_fail("invalid load value", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_out_of_bounds_minimal(ubsan_array_out_of_bounds_data* data)
    {
        ubsan_fail("array out of bounds", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_type_mismatch_minimal(ubsan_type_mismatch_v1_data* data, std::uintptr_t ptr)
    {
        if (!ptr)
        {
            ubsan_fail("null pointer", location_or_unk);
        }

        ubsan_fail("type mismatch", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_vla_bound_not_positive_minimal(ubsan_negative_vla_data* data)
    {
        ubsan_fail("variable-length argument is negative", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_nonnull_return_minimal(ubsan_nonnull_return_data* data)
    {
        ubsan_fail("non-null return is null", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_nonnull_arg_minimal(ubsan_nonnull_arg_data* data)
    {
        ubsan_fail("non-null argument is null", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_builtin_unreachable_minimal(ubsan_unreachable_data* data)
    {

        ubsan_fail("unreachable code reached", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_invalid_builtin_minimal(ubsan_invalid_builtin_data* data)
    {
        ubsan_fail("invalid builtin", location_or_unk);
    }

    NO_UBSAN void __ubsan_handle_alignment_assumption_minimal(void)
    {
        // todo: implement
        ubsan_fail("idk", UB_UNK);
    }
}
