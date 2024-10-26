#include "fence.h"
#include "vkassert.h"
#include <rune/core/logging.h>
#include <rune/core/alloc.h>

struct vkfence* create_vkfence(struct vkdev *dev, uint8_t signal) {
        struct vkfence *ret = rune_alloc(sizeof(struct vkfence));
        
        VkFenceCreateInfo fcinfo;
        fcinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fcinfo.pNext = NULL;
        if (signal == 1)
                fcinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        else
                fcinfo.flags = 0;
        vkassert(vkCreateFence(dev->ldev, &fcinfo, NULL, &ret->handle), "Failed to create fence");
        return ret;
}

void destroy_vkfence(struct vkfence *fence, struct vkdev *dev) {
        if (fence->handle != NULL) {
                vkDestroyFence(dev->ldev, fence->handle, NULL);
                fence->handle = NULL;
        }
        rune_free(fence);
}

uint8_t fence_lock(struct vkfence *fence, struct vkdev *dev, uint64_t timeout) {
        if (fence->signal)
                return 1;
        
        VkResult res = vkWaitForFences(dev->ldev, 1, &fence->handle, VK_TRUE, timeout);
        switch (res) {
                case VK_SUCCESS:
                        fence->signal = 1;
                        return 1;
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

        return 0;
}

void fence_unlock(struct vkfence *fence, struct vkdev *dev) {
        if (fence->signal == 0)
                return;

        vkassert(vkResetFences(dev->ldev, 1, &fence->handle), "Cannot reset Vulkan fence");
        fence->signal = 1;
}
