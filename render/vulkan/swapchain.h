#ifndef VKSWAPCHAIN_H
#define VKSWAPCHAIN_H

#include "surface.h"
#include "device.h"

struct vkswapchain {
        VkSwapchainKHR handle;
        VkSurfaceFormatKHR format_khr;
        VkFormat format;
        VkSemaphore render_complete;
        VkImage *images;
        VkImageView *views;
        struct vkimage *depth_attachment;
        uint8_t max_frames;
        uint32_t frame;
        uint32_t img_count;
};

struct vkswapchain* create_swapchain(struct vksurface *surface, struct vkdev *dev);
void destroy_swapchain(struct vkswapchain *swapchain, struct vkdev *dev);

void vkswapchain_present(struct vkswapchain *swapchain, struct vkdev *dev);

#endif
