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

#include "renderpass.h"
#include "vkassert.h"
#include <rune/core/alloc.h>

vkcmdbuffer_t* create_vkcmdbuffer(vkdev_t *dev, int primary) {
        vkcmdbuffer_t *ret = rune_calloc(0, sizeof(vkcmdbuffer_t));

        VkCommandBufferAllocateInfo ainfo;
        ainfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        ainfo.pNext = NULL;
        ainfo.commandPool = dev->cmd_pool;
        if (primary == 1)
                ainfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        else
                ainfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        ainfo.commandBufferCount = 1;
        vkassert(vkAllocateCommandBuffers(dev->ldev, &ainfo, &ret->handle));

        ret->state = CMDBUF_INITIAL;
        return ret;
}

void destroy_vkcmdbuffer(vkcmdbuffer_t *cmdbuffer, vkdev_t *dev) {
        vkFreeCommandBuffers(dev->ldev, dev->cmd_pool, 1, &cmdbuffer->handle);
        rune_free(cmdbuffer);
}

void cmdbuf_begin(vkcmdbuffer_t *cmdbuffer, int single, int rpass_cont, int sim_use) {
        if (cmdbuffer->state != CMDBUF_INITIAL) {
                log_output(LOG_FATAL, "Attempted to record to a command buffer not in initial state");
                rune_abort();
        }

        VkCommandBufferBeginInfo binfo;
        binfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        binfo.flags = 0;
        binfo.pNext = NULL;
        if (single)
                binfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (rpass_cont)
                binfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        if (sim_use)
                binfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        vkassert(vkBeginCommandBuffer(cmdbuffer->handle, &binfo));
        cmdbuffer->state = CMDBUF_RECORDING;
}

void cmdbuf_end(vkcmdbuffer_t *cmdbuffer) {
        if (cmdbuffer->state != CMDBUF_RECORDING) {
                log_output(LOG_FATAL, "Attempted to end command buffer not in recording state");
                rune_abort();
        }

        vkassert(vkEndCommandBuffer(cmdbuffer->handle));
        cmdbuffer->state = CMDBUF_READY;
}

void cmdbuf_submit(vkcmdbuffer_t *cmdbuffer, VkSemaphore *signal, VkSemaphore *wait, VkQueue queue_handle, VkFence fence_handle) {
        if (cmdbuffer->state != CMDBUF_READY) {
                log_output(LOG_FATAL, "Attempted to submit command buffer not in ready state");
                return;
        }

        VkSubmitInfo sinfo;
        sinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        sinfo.pNext = NULL;
        sinfo.commandBufferCount = 1;
        sinfo.pCommandBuffers = &cmdbuffer->handle;
        sinfo.signalSemaphoreCount = 1;
        sinfo.pSignalSemaphores = signal;
        sinfo.waitSemaphoreCount = 1;
        sinfo.pWaitSemaphores = wait;
        VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        sinfo.pWaitDstStageMask = flags;
        vkassert(vkQueueSubmit(queue_handle, 1, &sinfo, fence_handle));
        cmdbuffer->state = CMDBUF_SUBMITTED;
}

void cmdbuf_reset(vkcmdbuffer_t *cmdbuffer) {
        cmdbuffer->state = CMDBUF_INITIAL;
}

vkcmdbuffer_t* cmdbuf_begin_single_use(vkdev_t *dev) {
        vkcmdbuffer_t *ret = create_vkcmdbuffer(dev, 1);
        cmdbuf_begin(ret, 1, 0, 0);
        return ret;
}

void cmdbuf_end_single_use(vkcmdbuffer_t *cmdbuffer, vkdev_t *dev, VkQueue queue) {
        cmdbuf_end(cmdbuffer);
        VkSubmitInfo sinfo;
        sinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        sinfo.pNext = NULL;
        sinfo.waitSemaphoreCount = 0;
        sinfo.pWaitSemaphores = NULL;
        sinfo.pWaitDstStageMask = NULL;
        sinfo.commandBufferCount = 1;
        sinfo.pCommandBuffers = &cmdbuffer->handle;
        sinfo.signalSemaphoreCount = 0;
        sinfo.pSignalSemaphores = NULL;
        vkassert(vkQueueSubmit(queue, 1, &sinfo, 0));

        vkassert(vkQueueWaitIdle(queue));
        destroy_vkcmdbuffer(cmdbuffer, dev);
}

vkrendpass_t* create_vkrendpass(vkdev_t *dev, vkswapchain_t *swapchain, vec4 area, vec4 color, float depth, uint32_t stencil) {
        VkAttachmentDescription atdesc[2];
        atdesc[0].flags = 0;
        atdesc[0].format = swapchain->format_khr.format;
        atdesc[0].samples = VK_SAMPLE_COUNT_1_BIT;
        atdesc[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        atdesc[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        atdesc[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        atdesc[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        atdesc[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        atdesc[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        atdesc[1].flags = 0;
        atdesc[1].format = dev->depth_format;
        atdesc[1].samples = VK_SAMPLE_COUNT_1_BIT;
        atdesc[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        atdesc[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        atdesc[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        atdesc[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        atdesc[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        atdesc[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference cref;
        cref.attachment = 0;
        cref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference dref;
        dref.attachment = 1;
        dref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass;
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &cref;
        subpass.pDepthStencilAttachment = &dref;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = NULL;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = NULL;

        VkSubpassDependency dep;
        dep.srcSubpass = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass = 0;
        dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.srcAccessMask = 0;
        dep.dstAccessMask = 0;
        dep.dependencyFlags = 0;

        vkrendpass_t *ret = rune_alloc(sizeof(vkrendpass_t));
        ret->color[0] = color[0];
        ret->color[1] = color[1];
        ret->color[2] = color[2];
        ret->color[3] = color[3];
        ret->area[0] = area[0];
        ret->area[1] = area[1];
        ret->area[2] = area[2];
        ret->area[3] = area[3];
        ret->depth = depth;
        ret->stencil = stencil;
        VkRenderPassCreateInfo rcinfo;
        rcinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rcinfo.pNext = NULL;
        rcinfo.flags = 0;
        rcinfo.attachmentCount = 2;
        rcinfo.pAttachments = atdesc;
        rcinfo.subpassCount = 1;
        rcinfo.pSubpasses = &subpass;
        rcinfo.dependencyCount = 1;
        rcinfo.pDependencies = &dep;
        vkassert(vkCreateRenderPass(dev->ldev, &rcinfo, NULL, &ret->handle));

        log_output(LOG_DEBUG, "Initialized renderpass");
        return ret;
}

void destroy_vkrendpass(vkrendpass_t *rendpass, vkdev_t *dev) {
        if (rendpass->handle)
                vkDestroyRenderPass(dev->ldev, rendpass->handle, NULL);
        rune_free(rendpass);
}

void renderpass_begin(vkcmdbuffer_t *buf, vkrendpass_t *rendpass, VkFramebuffer framebuf) {
        if (buf->state != CMDBUF_RECORDING) {
                log_output(LOG_FATAL, "Attempted to place command buffer not in recording state in a render pass");
                rune_abort();
        }

        VkClearValue cvals[2];
        cvals[0].color.float32[0] = rendpass->color[0];
        cvals[0].color.float32[1] = rendpass->color[1];
        cvals[0].color.float32[2] = rendpass->color[2];
        cvals[0].color.float32[3] = rendpass->color[3];
        cvals[1].depthStencil.depth = rendpass->depth;
        cvals[1].depthStencil.stencil = rendpass->stencil;

        VkRenderPassBeginInfo binfo;
        binfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        binfo.pNext = NULL;
        binfo.renderPass = rendpass->handle;
        binfo.framebuffer = framebuf;
        binfo.renderArea.offset.x = rendpass->area[0];
        binfo.renderArea.offset.y = rendpass->area[1];
        binfo.renderArea.extent.width = rendpass->area[2];
        binfo.renderArea.extent.height = rendpass->area[3];
        binfo.clearValueCount = 2;
        binfo.pClearValues = cvals;

        vkCmdBeginRenderPass(buf->handle, &binfo, VK_SUBPASS_CONTENTS_INLINE);
        buf->state = CMDBUF_IN_RENDERPASS;
}

void renderpass_end(vkcmdbuffer_t *buf, vkrendpass_t *rendpass) {
        if (buf->state != CMDBUF_IN_RENDERPASS) {
                log_output(LOG_FATAL, "Attempted to purge command buffer not in render pass");
                rune_abort();
        }

        vkCmdEndRenderPass(buf->handle);
        buf->state = CMDBUF_RECORDING;
}
