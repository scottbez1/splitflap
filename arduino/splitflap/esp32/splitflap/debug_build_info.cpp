#include "Arduino.h"

#include "debug_build_info.h"

void logDebugBuildInfo(Logger& logger) {
    char buf[200];
    snprintf(buf, sizeof(buf), "build_git_hash: %s", BUILD_GIT_HASH);
    logger.log(buf);
    snprintf(buf, sizeof(buf), "build_date: %s", BUILD_DATE);
    logger.log(buf);
    snprintf(buf, sizeof(buf), "build_os: %s", BUILD_OS);
    logger.log(buf);
}
