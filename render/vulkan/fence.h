#ifndef VKFENCE_H
#define VKFENCE_H

#include "device.h"
#include <rune/util/types.h>
#include <vulkan/vulkan.h>

struct vkfence {
        VkFence handle;
        int signal;
};

struct vkfence* create_vkfence(struct vkdev *dev, uint8_t signal);
void destroy_vkfence(struct vkfence *fence, struct vkdev *dev);

uint8_t fence_lock(struct vkfence *fence, struct vkdev *dev, uint64_t timeout);
void fence_unlock(struct vkfence *fence, struct vkdev *dev);

#endif
