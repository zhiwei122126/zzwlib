
//
// per cpp has its own runtime loglevel and logtag
//

#include <stdio.h>
#include <string>

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

private:
    loglevel loglevel_ = loglevel::log_info_level;
    std::string tag_;
};
};

//========== default to use printf

#define LOGE(logger, fmt_str, args...) do { \
    if (logger.get_loglevel() > zzwlib::loglevel::log_err_level) \
        printf("[%s] %s : %d - " fmt_str "\n", logger.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGW(logger, fmt_str, args...) do { \
    if (logger.get_loglevel() > zzwlib::loglevel::log_warn_level) \
        printf("[%s] %s : %d - " fmt_str "\n",  logger.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGI(logger, fmt_str, args...) do { \
    if (logger.get_loglevel() > zzwlib::loglevel::log_info_level) \
        printf("[%s] %s : %d - " fmt_str "\n",  logger.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGD(logger, fmt_str, args...) do { \
    if (logger.get_loglevel() > zzwlib::loglevel::log_dgb_level) \
        printf("[%s] %s : %d - " fmt_str "\n",  logger.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)

#define LOGV(logger, fmt_str, args...) do { \
    if (logger.get_loglevel() > zzwlib::loglevel::log_verbose_level) \
        printf("[%s] %s : %d - " fmt_str "\n",  logger.get_tag(), __FUNCTION__, __LINE__, ##args); \
} while(0)
