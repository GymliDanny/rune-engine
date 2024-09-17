#include <rune/core/logging.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LSTR_FATAL      "[FATAL]"
#define LSTR_ERROR      "[ERROR]"
#define LSTR_WARN       "[WARNING]"
#define LSTR_INFO       "[INFO]"
#define LSTR_DEBUG      "[DEBUG]"

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
                        lvl_str = LSTR_DEBUG;
                        break;
        }
        printf("%s %s\n", lvl_str, out);
}
