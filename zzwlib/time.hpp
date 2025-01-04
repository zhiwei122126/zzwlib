
#pragma once

#include <exception>
#include <string>
#include <chrono>
#include <format>
#include <iostream>

namespace zzwlib {

class time_util final {
public:
     static std::string current_time_string () {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        auto local_tz = std::chrono::current_zone();
        std::chrono::zoned_time zt{local_tz, now};
        return std::format("{0:%F} {0:%T} ", zt.get_local_time());
    };

    static int64_t current_ms () {
        auto now = std::chrono::steady_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        return now_ms.time_since_epoch().count();
    };
private:
    // delete default construct
    time_util() = delete;
    //delete copy and move construct
    time_util(const time_util&) = delete;
    time_util& operator=(const time_util&) = delete;
    time_util(time_util&&) = delete;
    time_util& operator=(time_util&&) = delete;
};

};
