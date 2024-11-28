#ifndef VKDEVICE_H
#define VKDEVICE_H

#include "vk_types.h"

struct vkdev* create_vkdev(VkInstance instance, VkSurfaceKHR surface);
void destroy_vkdev(struct vkdev *dev);

void get_swapchain_data(struct vkdev *dev, VkSurfaceKHR *surface);
int get_depth_format(struct vkdev *dev);
uint32_t get_memory_index(struct vkdev *dev, uint32_t type, uint32_t flags);

#endif
