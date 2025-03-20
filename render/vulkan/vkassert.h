/*
 * Rune Game Engine
 * Copyright 2024 Danny Holman <dholman@gymli.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

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
