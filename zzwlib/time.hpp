

#pragma once

#include <exception>
#include <string>
#include <chrono>
#include <format>
#include <iostream>

namespace zzwlib {

auto current_time_string = []() -> std::string {
     std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
     auto local_tz = std::chrono::current_zone();
     std::chrono::zoned_time zt{local_tz, now};
     return std::format("{0:%F} {0:%T} ", zt.get_local_time());
};

auto current_ms = []() -> int {
    auto now = std::chrono::steady_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
};

};
