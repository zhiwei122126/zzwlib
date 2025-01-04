
//
// per cpp has its own runtime loglevel and logtag
//

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>

#ifdef LOGGER_TIMESTAMP_ENABLE
#include "time.hpp"
#endif

namespace zzwlib {

enum class loglevel : int {
    log_err_level = 10,
    log_warn_level = 20,
    log_info_level = 30,
    log_dgb_level = 40,
    log_verbose_level = 50,
};

class logger final {
public:
    logger(const std::string &tag, loglevel level) {
        tag_ = tag;
        loglevel_ = level;
    }

    logger(const std::string &tag) {
        tag_ = tag;
        loglevel_ = loglevel::log_info_level;
    }

    // Disable copy and move construct
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;
    logger(logger&&) = delete;
    logger& operator=(logger&&) = delete;

    loglevel get_loglevel() {return loglevel_;}

    const char * get_tag() {return tag_.c_str();}

    static int write_log(FILE* fp, const char *fmt_str, ...) {
#ifdef LOGGER_TIMESTAMP_ENABLE
        fprintf(fp, "%s", zzwlib::time_util::current_time_string().c_str());
#endif
        va_list args;
        va_start(args, fmt_str);
        int ret = vfprintf(fp, fmt_str, args);
        return ret;
    }

private:
    loglevel loglevel_ = loglevel::log_info_level;
    std::string tag_;
};
};

//========== default to use printf

#define LOGE(logger_obj, fmt_str, args...) do { \
    if (logger_obj.get_loglevel() > zzwlib::loglevel::log_err_level) \
        zzwlib::logger::write_log(stdout, "[%s] [err] %s : %d - " fmt_str "\n", logger_obj.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGW(logger_obj, fmt_str, args...) do { \
    if (logger_obj.get_loglevel() > zzwlib::loglevel::log_warn_level) \
        zzwlib::logger::write_log(stdout, "[%s] [warn] %s : %d - " fmt_str "\n", logger_obj.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGI(logger_obj, fmt_str, args...) do { \
    if (logger_obj.get_loglevel() > zzwlib::loglevel::log_info_level) \
        zzwlib::logger::write_log(stdout, "[%s] [info] %s : %d - " fmt_str "\n", logger_obj.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGD(logger_obj, fmt_str, args...) do { \
    if (logger_obj.get_loglevel() > zzwlib::loglevel::log_dgb_level) \
        zzwlib::logger::write_log(stdout, "[%s] [dbg] " fmt_str "\n", logger_obj.get_tag(), ##args); \
} while(0)

#define LOGV(logger_obj, fmt_str, args...) do { \
    if (logger_obj.get_loglevel() > zzwlib::loglevel::log_verbose_level) \
        zzwlib::logger::write_log(stdout, "[%s] [verbose] " fmt_str "\n", logger_obj.get_tag(), ##args); \
} while(0)
