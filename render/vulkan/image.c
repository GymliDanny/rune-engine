#include "image.h"
#include "vkassert.h"
#include <rune/core/alloc.h>

int _create_image_view(struct vkimage *image, struct vkdev *dev, VkFormat format, VkImageAspectFlags aflags) {
        VkImageViewCreateInfo vcinfo;
        vcinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vcinfo.pNext = NULL;
        vcinfo.flags = 0;
        vcinfo.image = image->handle;
        vcinfo.format = format;
        vcinfo.subresourceRange.aspectMask = aflags;
        vcinfo.subresourceRange.baseMipLevel = 0;
        vcinfo.subresourceRange.levelCount = 1;
        vcinfo.subresourceRange.baseArrayLayer = 0;
        vcinfo.subresourceRange.layerCount = 1;
        vkassert(vkCreateImageView(dev->ldev, &vcinfo, NULL, &image->view), "Failed to create image view");
}

struct vkimage* create_vkimage(struct vkdev *dev, VkFormat format, uint32_t width, uint32_t height, uint32_t usage, uint32_t mem_flags, uint32_t aflags, int create_view) {
        struct vkimage *ret = rune_alloc(sizeof(struct vkimage));
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
        vkassert(vkCreateImage(dev->ldev, &icinfo, NULL, &ret->handle), "Failed to create image");

        VkMemoryRequirements mem_req;
        vkGetImageMemoryRequirements(dev->ldev, ret->handle, &mem_req);

        int32_t mem_type = get_memory_index(dev, mem_req.memoryTypeBits, mem_flags);
        if (mem_type == -1)
                log_output(LOG_ERROR, "Image does not have a valid memory type");

        VkMemoryAllocateInfo mainfo;
        mainfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mainfo.allocationSize = mem_req.size;
        mainfo.memoryTypeIndex = mem_type;
        vkassert(vkAllocateMemory(dev->ldev, &mainfo, NULL, &ret->memory), "Failed to allocate image memory");
        vkassert(vkBindImageMemory(dev->ldev, ret->handle, ret->memory, 0), "Failed to bind image memory");

        if (create_view == 1)
                _create_image_view(ret, dev, format, aflags);

        return ret;
}

void destroy_vkimage(struct vkimage *image, struct vkdev *dev) {
        if (image->view)
                vkDestroyImageView(dev->ldev, image->view, NULL);
        if (image->memory)
                vkFreeMemory(dev->ldev, image->memory, NULL);
        if (image->handle)
                vkDestroyImage(dev->ldev, image->handle, NULL);
        rune_free(image);
}
