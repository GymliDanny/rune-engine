#ifndef VKFENCE_H
#define VKFENCE_H

#include "vk_types.h"

struct vkfence* create_vkfence(struct vkdev *dev, uint8_t signal);
void destroy_vkfence(struct vkfence *fence, struct vkdev *dev);

uint8_t fence_lock(struct vkfence *fence, struct vkdev *dev, uint64_t timeout);
void fence_unlock(struct vkfence *fence, struct vkdev *dev);

#endif
