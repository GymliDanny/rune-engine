#include <rune/core/abort.h>
#include <rune/core/init.h>
#include <rune/core/logging.h>
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
        exit(-1);
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
