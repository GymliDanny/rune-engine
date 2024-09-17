#include <rune/ui/window.h>
#include <rune/core/logging.h>
#include <rune/core/callbacks.h>
#include <rune/core/alloc.h>
#include <rune/core/abort.h>
#include <rune/util/types.h>
#include <string.h>

struct rune_window* rune_create_window(uint32_t width, uint32_t height, const char *title) {
        glfwInit();
        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        struct rune_window *ret = rune_alloc(sizeof(struct rune_window));
        ret->winw = width;
        ret->winh = height;
        ret->wintitle = title;
        ret->window = glfwCreateWindow(ret->winw, ret->winh,
                                       ret->wintitle,
                                       NULL, NULL);
        if (ret->window == NULL) {
                log_output(LOG_FATAL, "Cannot create window");
                rune_abort();
        }

        return ret;
}

void rune_destroy_window(struct rune_window *window) {
        glfwDestroyWindow(window->window);
        glfwTerminate();
}
