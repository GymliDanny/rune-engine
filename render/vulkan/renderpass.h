#ifndef VKRENDERPASS_H
#define VKRENDERPASS_H

#include "vk_types.h"

enum cmdbuf_state {
        CMDBUF_READY,
        CMDBUF_RECORD,
        CMDBUF_RPASS,
        CMDBUF_ENDREC,
        CMDBUF_SUBMIT
};

struct vkcmdbuffer* create_vkcmdbuffer(struct vkdev *dev, int primary);
void destroy_vkcmdbuffer(struct vkcmdbuffer *cmdbuffer, struct vkdev *dev);

void cmdbuf_begin(struct vkcmdbuffer *cmdbuffer, int single, int rpass_cont, int sim_use);
void cmdbuf_end(struct vkcmdbuffer *cmdbuffer);

struct vkrendpass* create_vkrendpass(struct vkdev *dev, struct vkswapchain *swapchain, vec4 area, vec4 color, float depth, uint32_t stencil);
void destroy_vkrendpass(struct vkrendpass *rendpass, struct vkdev *dev);

void renderpass_begin(struct vkcmdbuffer *buf, struct vkrendpass *rendpass, VkFramebuffer framebuf);
void renderpass_end(struct vkcmdbuffer *buf, struct vkrendpass *rendpass);

#endif
