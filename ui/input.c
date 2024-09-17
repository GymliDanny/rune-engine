#include <rune/ui/input.h>
#include <rune/ui/scancode.h>
#include <rune/core/callbacks.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>

static int keyboard_mode;
static void (*key_hooks[256])(void);

void _key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS && key_hooks[scancode] != NULL)
                (*key_hooks[scancode])();
}

int rune_input_init(struct rune_window *window) {
        keyboard_mode = KB_MODE_RAW;
        glfwSetKeyCallback(window->window, _key_callback);
        for (int i = 0; i < 256; i++)
                key_hooks[i] = NULL;
}

void set_keyboard_mode(int mode) {
        if (mode != KB_MODE_RAW || mode != KB_MODE_TEXT) {
                log_output(LOG_ERROR, "Attempted to set keyboard to invalid mode");
                return;
        }
        keyboard_mode = mode;
}

int get_keyboard_mode(void) {
        return keyboard_mode;
}

void register_key_hook(int key, void (*func)(void)) {
        key_hooks[key] = func;
}

void rune_input_tick(void) {
        glfwPollEvents();
}
