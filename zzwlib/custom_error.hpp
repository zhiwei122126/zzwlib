#pragma once

#include <string>
#include <string_view>

namespace zzwlib {
    class custom_error {

    public:
        enum class type : int {
            ok = 0,
            base_custom_error = 1000,
        };
        static int to_positive(int err) noexcept {
            if (err < 0) {
                return -err;
            }
            return err;
        }
        static std::string_view to_string(type err) noexcept {
            int positive_err_code = to_positive(static_cast<int>(err));

            if (positive_err_code < type::base_custom_error) {
                return strerror(static_cast<int>(err));
            }

            switch (positive_err_code) {
            case type::ok:
                return "ok";
            case type::base_custom_error:
                return "base_custom_error";
            }
            return "unknown";
        }
    }
}
