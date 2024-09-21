#include <rune/core/abort.h>
#include <rune/core/init.h>
#include <rune/core/logging.h>
#include <execinfo.h>
#include <stdlib.h>

#define MAX_TRACE_ITEMS 30

void _stack_trace(void) {
        void* buffer[MAX_TRACE_ITEMS];
        int num_links = backtrace(buffer, MAX_TRACE_ITEMS);
        char** symbuf = backtrace_symbols(buffer, num_links);

        for (int i = 0; i < num_links; i++)
                log_output(LOG_INFO, "#%d: %s", i, symbuf[i]);

        free(symbuf);
}

void rune_abort(void) {
        log_output(LOG_INFO, "Abort called, printing stack trace");
        _stack_trace();
        rune_exit();
}