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

#include "framebuffer.h"
#include "vkassert.h"
#include <rune/core/alloc.h>
#include <rune/core/logging.h>

vkframebuffer_t* create_vkframebuffer(vkdev_t *dev, vkrendpass_t *rendpass, uint32_t width, uint32_t height, uint32_t at_count, VkImageView *at) {
        vkframebuffer_t *ret = rune_alloc(sizeof(vkframebuffer_t));
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

void destroy_vkframebuffer(vkframebuffer_t *framebuffer, vkdev_t *dev) {
        vkDestroyFramebuffer(dev->ldev, framebuffer->handle, NULL);
        if (framebuffer->attachments)
                rune_free(framebuffer->attachments);
        rune_free(framebuffer);
}
