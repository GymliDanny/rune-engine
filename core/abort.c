#include <rune/core/abort.h>
#include <rune/core/init.h>
#include <rune/core/logging.h>
#include <stdlib.h>

#define MAX_TRACE_ITEMS 30

#ifdef _WIN32

void _stack_trace(void) {
        log_output(LOG_ERROR, "Stack tracing is not supported on Windows");
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

void rune_abort(void) {
        log_output(LOG_INFO, "Abort called, printing stack trace");
        _stack_trace();
        rune_exit();
}
