#ifndef VKFRAMEBUFFER_H
#define VKFRAMEBUFFER_H

#include "vk_types.h"

struct vkframebuffer* create_vkframebuffer(struct vkdev *dev, struct vkrendpass *rendpass, uint32_t width, uint32_t height, uint32_t at_count, VkImageView *at);
void destroy_vkframebuffer(struct vkframebuffer *framebuffer, struct vkdev *dev);

#endif
