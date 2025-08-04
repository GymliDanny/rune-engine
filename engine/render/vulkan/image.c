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

#include "image.h"
#include "device.h"
#include "vkassert.h"
#include <rune/core/alloc.h>

int _create_image_view(vkimage_t *image, vkdev_t *dev, VkFormat format, VkImageAspectFlags aflags) {
        VkImageViewCreateInfo vcinfo;
        vcinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vcinfo.pNext = NULL;
        vcinfo.flags = 0;
        vcinfo.image = image->handle;
        vcinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vcinfo.format = format;
        vcinfo.components.r = VK_COMPONENT_SWIZZLE_R;
        vcinfo.components.g = VK_COMPONENT_SWIZZLE_G;
        vcinfo.components.b = VK_COMPONENT_SWIZZLE_B;
        vcinfo.components.a = VK_COMPONENT_SWIZZLE_A;
        vcinfo.subresourceRange.aspectMask = aflags;
        vcinfo.subresourceRange.baseMipLevel = 0;
        vcinfo.subresourceRange.levelCount = 1;
        vcinfo.subresourceRange.baseArrayLayer = 0;
        vcinfo.subresourceRange.layerCount = 1;
        vkassert(vkCreateImageView(dev->ldev, &vcinfo, NULL, &image->view));
}

vkimage_t* create_vkimage(vkdev_t *dev, VkFormat format, uint32_t width, uint32_t height, uint32_t usage, uint32_t mem_flags, uint32_t aflags, int create_view) {
        vkimage_t *ret = rune_alloc(sizeof(vkimage_t));
        ret->width = width;
        ret->height = height;

        VkImageCreateInfo icinfo;
        icinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        icinfo.pNext = NULL;
        icinfo.flags = 0;
        icinfo.imageType = VK_IMAGE_TYPE_2D;
        icinfo.extent.width = width;
        icinfo.extent.height = height;
        icinfo.extent.depth = 1;
        icinfo.mipLevels = 1;
        icinfo.arrayLayers = 1;
        icinfo.format = format;
        icinfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        icinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        icinfo.usage = usage;
        icinfo.samples = VK_SAMPLE_COUNT_1_BIT;
        icinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkassert(vkCreateImage(dev->ldev, &icinfo, NULL, &ret->handle));

        VkMemoryRequirements mem_req;
        vkGetImageMemoryRequirements(dev->ldev, ret->handle, &mem_req);

        int32_t mem_type = get_memory_index(dev, mem_req.memoryTypeBits, mem_flags);
        if (mem_type == -1)
                log_output(LOG_ERROR, "Image does not have a valid memory type");

        VkMemoryAllocateInfo mainfo;
        mainfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mainfo.pNext = NULL;
        mainfo.allocationSize = mem_req.size;
        mainfo.memoryTypeIndex = mem_type;
        vkassert(vkAllocateMemory(dev->ldev, &mainfo, NULL, &ret->memory));
        vkassert(vkBindImageMemory(dev->ldev, ret->handle, ret->memory, 0));

        if (create_view == 1)
                _create_image_view(ret, dev, format, aflags);

        return ret;
}

void destroy_vkimage(vkimage_t *image, vkdev_t *dev) {
        if (image->view)
                vkDestroyImageView(dev->ldev, image->view, NULL);
        if (image->memory)
                vkFreeMemory(dev->ldev, image->memory, NULL);
        if (image->handle)
                vkDestroyImage(dev->ldev, image->handle, NULL);
        rune_free(image);
}
