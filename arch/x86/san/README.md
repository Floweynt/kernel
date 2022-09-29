# Kernel Sanitization Implementation 
This is the implementation of kernel space sanitizers. 
More specifically, it implements the undefined-behavior sanitizer handler stubs.  
There are plans for implementing KASAN  

All the sanitizers are the minimal version
## UB-sanitizer 
The currently implemented handlers:  
- `__ubsan_handle_add_overflow_minimal` 
- `__ubsan_handle_sub_overflow_minimal` 
- `__ubsan_handle_mul_overflow_minimal`
- `__ubsan_handle_divrem_overflow_minimal`
- `__ubsan_handle_negate_overflow_minimal`
- `__ubsan_handle_pointer_overflow_minimal`
- `__ubsan_handle_shift_out_of_bounds_minimal`
- `__ubsan_handle_load_invalid_value_minimal`
- `__ubsan_handle_out_of_bounds_minimal`
- `__ubsan_handle_type_mismatch_minimal`
- `__ubsan_handle_vla_bound_not_positive_minimal`
- `__ubsan_handle_nonnull_return_minimal`
- `__ubsan_handle_nonnull_arg_minimal`
- `__ubsan_handle_builtin_unreachable_minimal`
- `__ubsan_handle_invalid_builtin_minimal`
- `__ubsan_handle_alignment_assumption_minimal` - WIP

Related structures:
```cpp
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

```
