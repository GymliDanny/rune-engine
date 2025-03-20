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

#include "fence.h"
#include "vkassert.h"
#include <rune/core/logging.h>
#include <rune/core/alloc.h>

vkfence_t* create_vkfence(vkdev_t *dev, uint8_t signal) {
        vkfence_t *ret = rune_alloc(sizeof(vkfence_t));
        
        VkFenceCreateInfo fcinfo;
        fcinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fcinfo.pNext = NULL;
        if (signal == 1)
                fcinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        else
                fcinfo.flags = 0;
        vkassert(vkCreateFence(dev->ldev, &fcinfo, NULL, &ret->handle));
        return ret;
}

void destroy_vkfence(vkfence_t *fence, vkdev_t *dev) {
        if (fence->handle != NULL) {
                vkDestroyFence(dev->ldev, fence->handle, NULL);
                fence->handle = NULL;
        }
        rune_free(fence);
}

int fence_lock(vkfence_t *fence, vkdev_t *dev, uint64_t timeout) {
        if (fence->signal == 1)
                return 0;
        
        VkResult res = vkWaitForFences(dev->ldev, 1, &fence->handle, VK_TRUE, timeout);
        switch (res) {
                case VK_SUCCESS:
                        fence->signal = 1;
                        return 0;
                case VK_TIMEOUT:
                        log_output(LOG_WARN, "Vulkan fence timed out");
                        break;
                case VK_ERROR_DEVICE_LOST:
                        log_output(LOG_ERROR, "Lost access to host device");
                        break;
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                        log_output(LOG_ERROR, "Out of host memory");
                        break;
                case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        log_output(LOG_ERROR, "Out of device memory");
                        break;
                default:
                        log_output(LOG_ERROR, "Unknown error occurred on Vulkan fence");
                        break;
        }

        return -1;
}

void fence_unlock(vkfence_t *fence, vkdev_t *dev) {
        if (fence->signal == 0)
                return;

        vkassert(vkResetFences(dev->ldev, 1, &fence->handle));
        fence->signal = 0;
}
