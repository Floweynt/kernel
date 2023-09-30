#pragma once 

#include <cstddef>
namespace user::syscall 
{
    // async read/write
    auto sys_write() -> std::ssize_t;
    auto sys_read() -> std::ssize_t;

    // IO queue syscalls 
    auto sys_queue_io();
    
    // wait for async action 
    auto sys_await_async();
}
