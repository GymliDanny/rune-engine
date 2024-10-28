#include "renderpass.h"
#include "vkassert.h"
#include <rune/core/alloc.h>

struct vkcmdbuffer* create_vkcmdbuffer(struct vkdev *dev, int primary) {
        struct vkcmdbuffer *ret = rune_calloc(0, sizeof(struct vkcmdbuffer));

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

        ret->state = CMDBUF_READY;
        return ret;
}

void destroy_vkcmdbuffer(struct vkcmdbuffer *cmdbuffer, struct vkdev *dev) {
        vkFreeCommandBuffers(dev->ldev, dev->cmd_pool, 1, &cmdbuffer->handle);
        rune_free(cmdbuffer);
}

void cmdbuf_begin(struct vkcmdbuffer *cmdbuffer, int single, int rpass_cont, int sim_use) {
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
        cmdbuffer->state = CMDBUF_RECORD;
}

void cmdbuf_end(struct vkcmdbuffer *cmdbuffer) {
        vkassert(vkEndCommandBuffer(cmdbuffer->handle));
        cmdbuffer->state = CMDBUF_ENDREC;
}

struct vkcmdbuffer* cmdbuf_begin_single_use(struct vkdev *dev) {
        struct vkcmdbuffer *ret = create_vkcmdbuffer(dev, 1);
        cmdbuf_begin(ret, 1, 0, 0);
        return ret;
}

void cmdbuf_end_single_use(struct vkcmdbuffer *cmdbuffer, struct vkdev *dev, VkQueue queue) {
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

struct vkrendpass* create_vkrendpass(struct vkdev *dev, struct vkswapchain *swapchain, vec4 area, vec4 color, float depth, uint32_t stencil) {
        VkAttachmentDescription atdesc[2];
        atdesc[0].flags = 0;
        atdesc[0].format = swapchain->format;
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

        struct vkrendpass *ret = rune_alloc(sizeof(struct vkrendpass));
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

void destroy_vkrendpass(struct vkrendpass *rendpass, struct vkdev *dev) {
        if (rendpass->handle)
                vkDestroyRenderPass(dev->ldev, rendpass->handle, NULL);
        rune_free(rendpass);
}

void renderpass_begin(struct vkcmdbuffer *buf, struct vkrendpass *rendpass, VkFramebuffer framebuf) {
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
        buf->state = CMDBUF_RPASS;
}

void renderpass_end(struct vkcmdbuffer *buf, struct vkrendpass *rendpass) {
        vkCmdEndRenderPass(buf->handle);
        buf->state = CMDBUF_RECORD;
}
