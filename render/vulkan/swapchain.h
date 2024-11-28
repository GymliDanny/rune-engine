#ifndef VKSWAPCHAIN_H
#define VKSWAPCHAIN_H

#include "vk_types.h"

struct vkswapchain* create_swapchain(struct vksurface *surface, struct vkdev *dev);
void destroy_swapchain(struct vkswapchain *swapchain, struct vkdev *dev);

void vkswapchain_present(struct vkswapchain *swapchain, struct vkdev *dev);

#endif
