#pragma once

#include <string>
#include <string_view>

#include "base.hpp"

namespace zzwlib {
    class custom_error {

    public:
        enum class type : int {
            base_custom_error = 1000,
        };

        static std::string_view to_string(type err) noexcept {
            if (err < type::base_custom_error) {
                return base_err::to_string(err);
            }
            switch (err) {
            case type::base_custom_error:
                return "base_custom_error";
            }
            return "unknown";
        }
    }
}
