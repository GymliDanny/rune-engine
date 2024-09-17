#include <rune/core/init.h>
#include <rune/core/alloc.h>
#include <rune/core/logging.h>
#include <rune/ui/window.h>
#include <rune/ui/renderer.h>
#include <rune/ui/input.h>

static struct rune_window *window;
static struct rune_renderer *renderer;

struct rune_window* rune_init(uint32_t width, uint32_t height, const char *title) {
        log_output(LOG_INFO, "Started Rune Engine version %s", VERSION);
        window = rune_create_window(width, height, title);
        rune_input_init(window);
        renderer = rune_create_renderer(window, RUNE_RENDERER_VULKAN);
}

void rune_exit(void) {
        log_output(LOG_INFO, "Engine shutdown requested");
        rune_destroy_renderer(renderer);
        rune_destroy_window(window);
        rune_free_all();
}
