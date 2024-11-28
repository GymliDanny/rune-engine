#include "swapchain.h"
#include "image.h"
#include "device.h"
#include "vkassert.h"
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <rune/util/stubbed.h>

struct vkswapchain* create_swapchain(struct vksurface *surface, struct vkdev *dev) {
        struct vkswapchain *swapchain = rune_alloc(sizeof(struct vkswapchain));
        VkExtent2D sc_extent = {surface->width, surface->height};
        swapchain->max_frames = 2;
        get_swapchain_data(dev, &surface->handle);
        swapchain->format_khr = dev->scdata.formats[0];
        if (dev->scdata.capabilities.currentExtent.width != UINT32_MAX)
                sc_extent = dev->scdata.capabilities.currentExtent;

        VkExtent2D min = dev->scdata.capabilities.minImageExtent;
        VkExtent2D max = dev->scdata.capabilities.maxImageExtent;
        sc_extent.width = clamp(sc_extent.width, min.width, max.width);
        sc_extent.height = clamp(sc_extent.height, min.height, max.height);

        uint32_t img_count = dev->scdata.capabilities.minImageCount + 1;
        if (dev->scdata.capabilities.maxImageCount > 0 && img_count > dev->scdata.capabilities.maxImageCount)
                img_count = dev->scdata.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR cinfo;
        cinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        cinfo.surface = surface->handle;
        cinfo.minImageCount = img_count;
        cinfo.imageFormat = swapchain->format_khr.format;
        cinfo.imageColorSpace = swapchain->format_khr.colorSpace;
        cinfo.imageExtent = sc_extent;
        cinfo.imageArrayLayers = 1;
        cinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        cinfo.preTransform = dev->scdata.capabilities.currentTransform;
        cinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        cinfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        cinfo.clipped = VK_TRUE;
        cinfo.oldSwapchain = NULL;
        if (dev->queues[0].index != dev->queues[3].index) {
                uint32_t qfams[] = {dev->queues[0].index, dev->queues[3].index};
                cinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                cinfo.queueFamilyIndexCount = 2;
                cinfo.pQueueFamilyIndices = qfams;
        } else {
                cinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                cinfo.queueFamilyIndexCount = 0;
                cinfo.pQueueFamilyIndices = 0;
        }
        vkassert(vkCreateSwapchainKHR(dev->ldev, &cinfo, NULL, &swapchain->handle));
        vkassert(vkGetSwapchainImagesKHR(dev->ldev, swapchain->handle, &swapchain->img_count, NULL));

        swapchain->images = rune_alloc(sizeof(VkImage) * swapchain->img_count);
        swapchain->views = rune_alloc(sizeof(VkImageView) * swapchain->img_count);
        vkassert(vkGetSwapchainImagesKHR(dev->ldev, swapchain->handle, &swapchain->img_count, swapchain->images));

        VkImageViewCreateInfo vcinfo;
        for (uint32_t i = 0; i < swapchain->img_count; i++) {
                vcinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                vcinfo.pNext = NULL;
                vcinfo.image = swapchain->images[i];
                vcinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                vcinfo.format = swapchain->format_khr.format;
                vcinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                vcinfo.subresourceRange.baseMipLevel = 0;
                vcinfo.subresourceRange.levelCount = 1;
                vcinfo.subresourceRange.baseArrayLayer = 0;
                vcinfo.subresourceRange.layerCount = 1;
                vkassert(vkCreateImageView(dev->ldev, &vcinfo, NULL, &swapchain->views[i]));
        }

        if (get_depth_format(dev) == 0) {
                log_output(LOG_FATAL, "Failed to find a supported image format");
                rune_abort();
        }

        swapchain->depth_attachment = create_vkimage(dev,
                                                     dev->depth_format,
                                                     surface->width,
                                                     surface->height,
                                                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                     VK_IMAGE_ASPECT_DEPTH_BIT,
                                                     1);
        swapchain->frame = 0;
        log_output(LOG_DEBUG, "Initialized swapchain");
        return swapchain;
}

void destroy_swapchain(struct vkswapchain *swapchain, struct vkdev *dev) {
        for (uint32_t i = 0; i < swapchain->img_count; i++)
                vkDestroyImageView(dev->ldev, swapchain->views[i], NULL);
        destroy_vkimage(swapchain->depth_attachment, dev);
        vkDestroySwapchainKHR(dev->ldev, swapchain->handle, NULL);
        rune_free(swapchain->images);
        rune_free(swapchain->views);
}

void vkswapchain_present(struct vkswapchain *swapchain, struct vkdev *dev) {
        VkPresentInfoKHR pinfo;
        pinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        pinfo.pNext = NULL;
        pinfo.waitSemaphoreCount = 1;
        pinfo.pWaitSemaphores = &swapchain->render_complete;
        pinfo.swapchainCount = 1;
        pinfo.pSwapchains = &swapchain->handle;
        pinfo.pImageIndices = &dev->queues[3].index;
        pinfo.pResults = NULL;

        VkResult res = vkQueuePresentKHR(dev->queues[3].handle, &pinfo);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
                STUBBED("Recreate swapchain");
        else if (res != VK_SUCCESS)
                log_output(LOG_ERROR, "Vulkan error: %s", get_vkerr_str(res));

        swapchain->frame = (swapchain->frame + 1) % swapchain->max_frames;
}
