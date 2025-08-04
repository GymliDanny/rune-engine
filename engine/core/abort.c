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

#include <rune/core/abort.h>
#include <rune/core/alloc.h>
#include <rune/core/init.h>
#include <rune/core/logging.h>
#include <rune/util/exits.h>
#include <stdlib.h>

#define MAX_TRACE_ITEMS 30

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

void _stack_trace(void) {
        void* buffer[MAX_TRACE_ITEMS];
        HANDLE process = GetCurrentProcess();

        SymInitialize(process, NULL, TRUE);
        int num_links = CaptureStackBackTrace(0, MAX_TRACE_ITEMS, buffer, NULL);
        SYMBOL_INFO *symbol = (SYMBOL_INFO*)rune_alloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;

        for (int i = 0; i < num_links; i++) {
                SymFromAddr(process, (DWORD64)(buffer[i]), 0, symbol);
                log_output(LOG_INFO, "#%d: %s", i, symbol->Name, symbol->Address);
        }

        rune_free(symbol);
        SymCleanup(process);
}

#else

#include <execinfo.h>

void _stack_trace(void) {
        void* buffer[MAX_TRACE_ITEMS];
        int num_links = backtrace(buffer, MAX_TRACE_ITEMS);
        char** symbuf = backtrace_symbols(buffer, num_links);

        for (int i = 0; i < num_links; i++)
                log_output(LOG_INFO, "#%d: %s", i, symbuf[i]);

        free(symbuf);
}

#endif

NORET void rune_abort(void) {
        log_output(LOG_INFO, "Abort called, printing stack trace");
        _stack_trace();
        rune_exit();
        exit(REXIT_FAIL);
}

#ifdef MSVC

NORET void __security_error_handler(void) {
        log_output(LOG_FATAL, "Stack smashing detected in engine code");
        rune_abort();
}

#else

NORET void __stack_chk_fail(void) {
        log_output(LOG_FATAL, "Stack smashing detected in engine code");
        rune_abort();
}

#endif
