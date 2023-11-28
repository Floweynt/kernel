#pragma once
#include <process/context.h>

extern "C" [[noreturn]] void return_to_context(proc::context* ptr);
