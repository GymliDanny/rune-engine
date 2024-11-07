#include <rune/ui/app_window.h>
#include <rune/core/abort.h>
#include <rune/core/alloc.h>
#include <rune/core/callbacks.h>
#include <rune/core/config.h>
#include <rune/core/logging.h>
#include <rune/util/types.h>
#include <string.h>

struct rune_window* rune_init_window(void) {
        glfwInit();
        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        struct rune_window *ret = rune_alloc(sizeof(struct rune_window));
        ret->winw = 1920;
        ret->winh = 1080;
        ret->wintitle = rune_get_app_name();
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
