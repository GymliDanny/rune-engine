#include <rune/ui/renderer.h>
#include <rune/video/vulkan/renderer.h>
#include <rune/video/directx/renderer.h>
#include <rune/core/alloc.h>
#include <rune/core/logging.h>
#include <rune/core/abort.h>

struct rune_renderer* rune_create_renderer(struct rune_window *window, int type) {
        struct rune_renderer *ret = rune_alloc(sizeof(struct rune_renderer));
        ret->window = window;

        switch (type) {
                case RUNE_RENDERER_VULKAN:
                        ret->init = rune_init_vulkan;
                        ret->close = rune_close_vulkan;
                        break;
                case RUNE_RENDERER_DIRECTX:
                        ret->init = rune_init_directx;
                        ret->close = rune_close_directx;
                        break;
                default:
                        log_output(LOG_FATAL, "Invalid rendering API selected");
                        rune_abort();
        }

        (*ret->init)(ret);
}

void rune_destroy_renderer(struct rune_renderer *renderer) {
        (*renderer->close)(renderer);
        rune_free(renderer);
}
