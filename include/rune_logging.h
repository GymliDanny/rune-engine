#ifndef RUNE_LOGGING_H
#define RUNE_LOGGING_H

#include <rune_types.h>

enum log_level {
        LOG_FATAL,
        LOG_ERROR,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG
};

void log_output(int level, const char *fmt, ...);

#endif
