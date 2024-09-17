#include <rune/core/callbacks.h>
#include <stdio.h>

void error_callback(int error, const char *desc) {
        fprintf(stderr, "Error %d: %s\n", error, desc);
}
