
#pragma once

#include <exception>

namespace zzwlib {

class custom_exception : std::exception {
public:
    custom_exception(int error_code, const char * msg) :
        error_code_(error_code),
        msg_(msg) {
    }
    custom_exception(int error_code) :
        error_code_(error_code),
        msg_(nullptr) {
    }
    // copy constructor and copy assignment with default
    custom_exception(const custom_exception&) = default;
    custom_exception& operator=(const custom_exception&) = default;
    // move constructor and move assignment with default
    custom_exception(const custom_exception&&) = default;
    custom_exception& operator=(const custom_exception&&) = default;

    const char * get_msg() const {
        return msg_;
    }
private:
    int error_code_ = 0;
    const char *msg_ = nullptr;
};

};

