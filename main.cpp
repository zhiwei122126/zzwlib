
//#define LOGGER_TIMESTAMP_ENABLE

#include <iostream>
#include <memory>
#include <inttypes.h>

#include "zzwlib/logger.hpp"
#include "zzwlib/unique_handle.hpp"
#include "zzwlib/time.hpp"
#include "drm_test.hpp"

zzwlib::logger logger("main", zzwlib::loglevel::log_verbose_level);

int main(void){
    LOGD(logger, "beg drm test at %" PRIi64 " ms" , zzwlib::time_util::current_ms());
    drm_test();
    LOGD(logger, "end drm test at %" PRIi64 " ms", zzwlib::time_util::current_ms());
    LOGD(logger, "normal exit");
    return 0;
}
