#include <rune/core/callbacks.h>
#include <rune/core/logging.h>

void error_callback(int error, const char *desc) {
        log_output(LOG_ERROR, "%d: %s\n", error, desc);
}
