#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fs/vfs.h>
#include <klog/klog.h>
#include <string_view>
#include <unordered_map>

auto load(void* data, std::size_t size)-> std::refcounted<vfs::vfs>;
