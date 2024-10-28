#include "framebuffer.h"
#include "vkassert.h"
#include <rune/core/alloc.h>
#include <rune/core/logging.h>

struct vkframebuffer* create_vkframebuffer(struct vkdev *dev, struct vkrendpass *rendpass, uint32_t width, uint32_t height, uint32_t at_count, VkImageView *at) {
        struct vkframebuffer *ret = rune_alloc(sizeof(struct vkframebuffer));
        ret->at_count = at_count;
        ret->attachments = rune_alloc(sizeof(VkImageView) * at_count);
        for (uint32_t i = 0; i < at_count; i++)
                ret->attachments[i] = at[i];
        ret->rendpass = rendpass;
        
        VkFramebufferCreateInfo fbinfo;
        fbinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbinfo.pNext = NULL;
        fbinfo.flags = 0;
        fbinfo.renderPass = rendpass->handle;
        fbinfo.attachmentCount = ret->at_count;
        fbinfo.pAttachments = ret->attachments;
        fbinfo.width = width;
        fbinfo.height = height;
        fbinfo.layers = 1;
        vkassert(vkCreateFramebuffer(dev->ldev, &fbinfo, NULL, &ret->handle));

        return ret;
}

void destroy_vkframebuffer(struct vkframebuffer *framebuffer, struct vkdev *dev) {
        vkDestroyFramebuffer(dev->ldev, framebuffer->handle, NULL);
        if (framebuffer->attachments)
                rune_free(framebuffer->attachments);
        rune_free(framebuffer);
}
