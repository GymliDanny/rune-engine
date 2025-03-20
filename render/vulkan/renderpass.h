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

#ifndef VKRENDERPASS_H
#define VKRENDERPASS_H

#include "vk_types.h"

enum cmdbuf_state {
        CMDBUF_INITIAL,
        CMDBUF_RECORDING,
        CMDBUF_IN_RENDERPASS,
        CMDBUF_READY,
        CMDBUF_SUBMITTED,
};

vkcmdbuffer_t* create_vkcmdbuffer(vkdev_t *dev, int primary);
void destroy_vkcmdbuffer(vkcmdbuffer_t *cmdbuffer, vkdev_t *dev);

void cmdbuf_begin(vkcmdbuffer_t *cmdbuffer, int single, int rpass_cont, int sim_use);
void cmdbuf_end(vkcmdbuffer_t *cmdbuffer);
void cmdbuf_submit(vkcmdbuffer_t *cmdbuffer, VkSemaphore *signal, VkSemaphore *wait, VkQueue queue_handle, VkFence fence_handle);
void cmdbuf_reset(vkcmdbuffer_t *cmdbuffer);

vkrendpass_t* create_vkrendpass(vkdev_t *dev, vkswapchain_t *swapchain, vec4 area, vec4 color, float depth, uint32_t stencil);
void destroy_vkrendpass(vkrendpass_t *rendpass, vkdev_t *dev);

void renderpass_begin(vkcmdbuffer_t *buf, vkrendpass_t *rendpass, VkFramebuffer framebuf);
void renderpass_end(vkcmdbuffer_t *buf, vkrendpass_t *rendpass);

#endif
