#ifndef VKASSERT_H
#define VKASSERT_H

#include "vk_types.h"
#include <rune/core/logging.h>
#include <rune/core/abort.h>
#include <string.h>

static char* get_vkerr_str(VkResult res) {
        char *ret;
        switch (res) {
                case VK_SUCCESS:
                        ret = "SUCCESS";
                        break;
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                        ret = "OUT OF HOST MEMORY";
                        break;
                case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        ret = "OUT OF DEVICE MEMORY";
                        break;
                case VK_ERROR_INITIALIZATION_FAILED:
                        ret = "INITIALIZATION FAILED";
                        break;
                case VK_ERROR_LAYER_NOT_PRESENT:
                        ret = "VALIDATION LAYER NOT PRESENT";
                        break;
                case VK_ERROR_EXTENSION_NOT_PRESENT:
                        ret = "EXTENSION NOT PRESENT";
                        break;
                case VK_ERROR_INCOMPATIBLE_DRIVER:
                        ret = "INCOMPATIBLE DRIVER";
                        break;
                default:
                        ret = "UNKNOWN RESULT";
                        break;
        }
        return strdup(ret);
}

static inline void vkassert(VkResult value) {
        if (value != VK_SUCCESS) {
                log_output(LOG_FATAL, "Vulkan error: %s", get_vkerr_str(value));
                rune_abort();
        }
}

#endif
