#include <rune/core/logging.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LSTR_FATAL      "\033[31m[FATAL]"
#define LSTR_ERROR      "\033[31m[ERROR]"
#define LSTR_WARN       "\033[33m[WARNING]"
#define LSTR_INFO       "\033[0m[INFO]"
#define LSTR_DEBUG      "\033[32m[DEBUG]"

static int debug_enabled = 0;

void log_output(int level, const char *fmt, ...) {
        char out[8192];
        memset(out, 0, sizeof(out));

        va_list arg_ptr;
        va_start(arg_ptr, fmt);
        vsnprintf(out, 8192, fmt, arg_ptr);
        va_end(arg_ptr);

        char *lvl_str;
        switch (level) {
                case LOG_FATAL:
                        lvl_str = LSTR_FATAL;
                        break;
                case LOG_ERROR:
                        lvl_str = LSTR_ERROR;
                        break;
                case LOG_WARN:
                        lvl_str = LSTR_WARN;
                        break;
                case LOG_INFO:
                        lvl_str = LSTR_INFO;
                        break;
                case LOG_DEBUG:
                        if (debug_enabled == 0)
                                return;
                        lvl_str = LSTR_DEBUG;
                        break;
        }
        printf("%s %s\n%s", lvl_str, out, "\033[0m");
}

void enable_log_debug(void) {
        debug_enabled = 1;
}

void disable_log_debug(void) {
        debug_enabled = 0;
}
