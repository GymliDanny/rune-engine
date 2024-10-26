#ifndef VKIMAGE_H
#define VKIMAGE_H

#include <rune/util/types.h>
#include "surface.h"
#include "device.h"

struct vkimage {
        VkImage handle;
        VkDeviceMemory memory;
        VkImageView view;
        uint32_t width;
        uint32_t height;
};

struct vkimage* create_vkimage(struct vkdev *dev, VkFormat format, uint32_t width, uint32_t height, uint32_t usage, uint32_t mem_flags, uint32_t aflags, int create_view);
void destroy_vkimage(struct vkimage *image, struct vkdev *dev);

#endif
