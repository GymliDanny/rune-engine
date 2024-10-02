#include <rune/core/logging.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define COLOR_NONE      ""
#define COLOR_DEFAULT   "\033[0m"
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"

#define LSTR_FATAL      "[FATAL]"
#define LSTR_ERROR      "[ERROR]"
#define LSTR_WARN       "[WARNING]"
#define LSTR_INFO       "[INFO]"
#define LSTR_DEBUG      "[DEBUG]"

static int debug_enabled = 0;
static int color_enabled = 1;

void log_output(int level, const char *fmt, ...) {
        char out[4096];
        memset(out, 0, sizeof(out));

        va_list arg_ptr;
        va_start(arg_ptr, fmt);
        vsnprintf(out, 4096, fmt, arg_ptr);
        va_end(arg_ptr);

        char *lvl_str;
        char *color = COLOR_NONE;
        switch (level) {
                case LOG_FATAL:
                        if (color_enabled == 1)
                                color = COLOR_RED;
                        lvl_str = LSTR_FATAL;
                        break;
                case LOG_ERROR:
                        if (color_enabled == 1)
                                color = COLOR_RED;
                        lvl_str = LSTR_ERROR;
                        break;
                case LOG_WARN:
                        if (color_enabled == 1)
                                color = COLOR_YELLOW;
                        lvl_str = LSTR_WARN;
                        break;
                case LOG_INFO:
                        lvl_str = LSTR_INFO;
                        break;
                case LOG_DEBUG:
                        if (color_enabled == 1)
                                color = COLOR_GREEN;
                        if (debug_enabled == 0)
                                return;
                        lvl_str = LSTR_DEBUG;
                        break;
        }

        if (color_enabled == 0)
                printf("%s %s\n", lvl_str, out);
        else
                printf("%s%s %s\n%s", color, lvl_str, out, COLOR_DEFAULT);
}

void enable_log_debug(void) {
        debug_enabled = 1;
}

void disable_log_debug(void) {
        debug_enabled = 0;
}

void enable_log_color(void) {
        color_enabled = 1;
}

void disable_log_color(void) {
        color_enabled = 0;
}
