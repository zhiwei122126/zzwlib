

#pragma once

#include <exception>

namespace zzwlib {

auto current_time_string = []() -> std::string { return std::format("{0:%F} {0:%T}", zt.get_local_time());}

auto current_ms = [] -> int { auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
                      auto value = now_ms.time_since_epoch().count();
                      return value;
     }


};