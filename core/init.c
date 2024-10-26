#include <rune/core/init.h>
#include <rune/core/alloc.h>
#include <rune/core/abort.h>
#include <rune/core/logging.h>
#include <rune/core/thread.h>

int rune_init(int argc, char* argv[]) {
        enable_log_color();
        log_output(LOG_INFO, "Started Rune Engine version %s", VERSION);
        rune_init_thread_api();
        return 0;
}

void rune_exit(void) {
        log_output(LOG_INFO, "Engine shutdown requested");
        rune_free_all();
}
