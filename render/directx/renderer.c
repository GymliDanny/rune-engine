#include <rune/ui/renderer.h>
#include <rune/core/logging.h>
#include <rune/core/abort.h>

#ifdef _WIN32

struct rune_renderer* select_render_directx(struct rune_window *window) {
        log_output(LOG_FATAL, "DirectX support has not yet been implemented");
        rune_abort();
}

#else

struct rune_renderer* select_render_directx(struct rune_window *window) {
        log_output(LOG_FATAL, "DirectX is not supported on this platform");
        rune_abort();
}

#endif
