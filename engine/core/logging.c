/*
 * Rune Game Engine
 * Copyright 2024 Danny Holman <dholman@gymli.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <rune/core/logging.h>
#include <rune/core/config.h>
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
static int color_enabled = 0;

void log_output(int level, const char *fmt, ...) {
        char out[4096];
        memset(out, 0, sizeof(out));

        debug_enabled = rune_get_log_debug();
        color_enabled = rune_get_log_color();

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

        if (color_enabled == 0) {
                printf("%s %s\n", lvl_str, out);
        } else {
                printf("%s%s %s\n", color, lvl_str, out);
                printf(COLOR_DEFAULT);
        }
}

//#ifdef _WIN32
//
//#include <windows.h>
//
//void enable_log_color(void) {
//        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
//        SetConsoleMode(handle, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
//        color_enabled = 1;
//}
//
//#else
//
//void enable_log_color(void) {
//        color_enabled = 1;
//}
//
//#endif
//
//void disable_log_color(void) {
//        color_enabled = 0;
//}
