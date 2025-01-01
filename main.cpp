
#include <iostream>
#include <memory>

#include "zzwlib/logger.hpp"
#include "zzwlib/unique_handle.hpp"

zzwlib::logger logger("main", zzwlib::loglevel::log_verbose_level);

int main(void){
    LOGD(logger, "add result is %d \n", add(2,3) );
    LOGD(logger, "mul result is %d \n", mul(2,3) );
    return 0;
}

