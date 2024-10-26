#ifndef VKASSERT_H
#define VKASSERT_H

#include <rune/core/logging.h>
#include <rune/core/abort.h>
#include <vulkan/vulkan.h>

static inline void vkassert(VkResult value, const char *str) {
        if (value != VK_SUCCESS) {
                log_output(LOG_ERROR, "Vulkan assertion failed: %s", str);
                rune_abort();
        }
}

#endif
