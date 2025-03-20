#include <rune/ui/input.h>
#include <rune/ui/scancode.h>
#include <rune/core/callbacks.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <string.h>

typedef struct callback {
        void *data;
        void (*callback_ptr)(void*);
} callback_t;

static int keyboard_mode;
static callback_t callbacks[256];

void _key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if ((action == GLFW_PRESS) && (callbacks[scancode].callback_ptr != NULL))
                (*callbacks[scancode].callback_ptr)(callbacks[scancode].data);
}

int rune_input_init(window_t *window) {
        keyboard_mode = KB_MODE_RAW;
        glfwSetKeyCallback(window->window, _key_callback);
        for (int i = 0; i < 256; i++)
                memset(&callbacks[i], 0, sizeof(callback_t));
        log_output(LOG_DEBUG, "Initialized keyboard input");
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

void register_input_callback(uint8_t scancode, void (*func)(void*), void *data) {
        callbacks[scancode].data = data;
        callbacks[scancode].callback_ptr = func;
        log_output(LOG_DEBUG, "Registered scancode %d with callback function address %x", scancode, func);
}

void rune_input_tick(void) {
        glfwPollEvents();
}
