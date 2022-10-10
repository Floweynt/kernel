#include <cstddef>
#include <cstdint>

enum {
    REPORT_ERROR_OPCODE,
    READ_OPCODE,
    WRITE_OPCODE,
    OPEN_OPCODE,
    WAKEUP_OPCODE
};

struct complete_next_normal_header 
{
    std::uint32_t id;
    std::uint32_t next;
    std::uint32_t prev;
};

struct complete_next_operation
{
    complete_next_normal_header header;
    char unknown[16 + 4];
};

struct report_error_operation
{
    const complete_next_normal_header header = {
        .id = REPORT_ERROR_OPCODE,
        .next = -1u,
        .prev = -1u
    };

    std::uint32_t error_additional;
    std::uint32_t full_error;
    std::uint32_t padding;
    std::uint64_t reserved;
};

struct read_operation
{
    const complete_next_normal_header header = {
        .id = READ_OPCODE,
    };

    std::uint32_t fd;
    std::size_t len;
    void* buffer;
};

struct write_operation
{
    const complete_next_normal_header header = {
        .id = WRITE_OPCODE,
    };

    std::uint32_t fd;
    std::size_t len;
    void* buffer;
};

struct open_operation
{
    const complete_next_normal_header header = {
        .id = OPEN_OPCODE,
    };

    std::uint32_t flags;
    const char* filename;
    std::uint32_t* target;
};

struct wakeup_thread
{
    const complete_next_normal_header header = {
        .id = WAKEUP_OPCODE
    };

    std::uint32_t target_tid;
    std::uint64_t reserved[2];
};

static_assert(sizeof(complete_next_operation) == 32);
static_assert(sizeof(report_error_operation) == 32);
static_assert(sizeof(read_operation) == 32);
static_assert(sizeof(open_operation) == 32);
static_assert(sizeof(wakeup_thread) == 32);

// The syscall
// i64 sys_enqueue_complete_next(int flags, std::size_t n, complete_next_operation* buffer)
// Returns a "task_queue_id", that is either the error id (or 0)
// Or, it returns a deferred ID
// i64 sys_await_complete_next(i64)
