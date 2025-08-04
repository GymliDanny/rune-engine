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

#include "vk_types.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "swapchain.h"
#include "device.h"
#include "context.h"
#include "image.h"
#include "fence.h"
#include "vkassert.h"
#include <rune/render/renderer.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <rune/core/abort.h>
#include <rune/core/config.h>
#include <sys/time.h>

static vkcontext_t *context = NULL;

void _init_cmdbuffers(void) {
        uint32_t num_buffers = context->swapchain->img_count;
        if (context->cmdbuffers == NULL)
                context->cmdbuffers = rune_calloc(0, sizeof(vkcmdbuffer_t*) * num_buffers);

        for (uint32_t i = 0; i < num_buffers; i++) {
                if (context->cmdbuffers[i] != NULL)
                        destroy_vkcmdbuffer(context->cmdbuffers[i], context->dev);
                context->cmdbuffers[i] = create_vkcmdbuffer(context->dev, 1);
        }
        log_output(LOG_DEBUG, "Created %d command buffers", num_buffers);
}

void _destroy_cmdbuffers(void) {
        uint32_t num_buffers = context->swapchain->img_count;
        if (context->cmdbuffers == NULL)
                return;
        for (uint32_t i = 0; i < num_buffers; i++) {
                if (context->cmdbuffers[i] != NULL)
                        destroy_vkcmdbuffer(context->cmdbuffers[i], context->dev);
        }
        log_output(LOG_DEBUG, "Destroyed %d command buffers", num_buffers);
}

void _init_framebuffers(void) {
        uint32_t num_buffers = context->swapchain->img_count;
        if (context->framebuffers == NULL)
                context->framebuffers = rune_calloc(0, sizeof(vkframebuffer_t*) * num_buffers);

        uint32_t at_count = 2;
        for (uint32_t i = 0; i < num_buffers; i++) {
                VkImageView attachments[at_count];
                attachments[0] = context->swapchain->views[i];
                attachments[1] = context->swapchain->depth_attachment->view;

                if (context->framebuffers[i] != NULL)
                        destroy_vkframebuffer(context->framebuffers[i], context->dev);
                context->framebuffers[i] = create_vkframebuffer(context->dev,
                                                                context->rendpass,
                                                                context->surface->width,
                                                                context->surface->height,
                                                                at_count,
                                                                attachments);
        }
        log_output(LOG_DEBUG, "Created %d frame buffers", num_buffers);
}

void _destroy_framebuffers(void) {
        uint32_t num_buffers = context->swapchain->img_count;
        if (context->framebuffers == NULL)
                return;

        for (uint32_t i = 0; i < num_buffers; i++)
                destroy_vkframebuffer(context->framebuffers[i], context->dev);
        log_output(LOG_DEBUG, "Destroyed %d frame buffers", num_buffers);
}

int _init_vulkan(window_t *window) {
        log_output(LOG_DEBUG, "Initializing Vulkan");
        struct timeval start;
        struct timeval stop;
        gettimeofday(&start, NULL);

        ext_container_t ext;
        ext.extensions = glfwGetRequiredInstanceExtensions(&ext.ext_count);
        
        if (rune_get_vk_debug() == 1) {
                vklayer_container_t *vklayers = init_vklayers(&ext);
                context = create_vkcontext(vklayers, &ext);
        } else {
                context = create_vkcontext(NULL, &ext);
        }

        if (context == NULL)
                return -1;

        VkResult res = glfwCreateWindowSurface(context->instance,
                                               window->window,
                                               NULL,
                                               &context->surface->handle);
        if (res != VK_SUCCESS) {
                log_output(LOG_FATAL, "Cannot create rendering surface");
                return -1;
        }

        context->dev = create_vkdev(context->instance,
                                    context->surface->handle,
                                    1,
                                    1,
                                    1,
                                    1);
        if (context->dev == NULL)
                return -1;

        context->surface->width = window->winw;
        context->surface->height = window->winh;
        context->swapchain = create_swapchain(context->surface, context->dev);
        if (context->swapchain == NULL)
                return -1;

        vec4 area = {0, 0, context->surface->width, context->surface->height};
        vec4 color = {0, 0, 0, 1.0f};
        context->rendpass = create_vkrendpass(context->dev, context->swapchain, area, color, 1.0, 0);
        if (context->rendpass == NULL)
                return -1;

        _init_framebuffers();
        _init_cmdbuffers();

        context->image_semaphores = rune_alloc(sizeof(VkSemaphore) * context->swapchain->max_frames);
        context->queue_semaphores = rune_alloc(sizeof(VkSemaphore) * context->swapchain->max_frames);
        context->fences_in_flight = rune_calloc(0, sizeof(vkfence_t*) * context->swapchain->max_frames);

        VkSemaphoreCreateInfo scinfo;
        for (uint8_t i = 0; i < context->swapchain->max_frames; i++) {
                scinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                scinfo.pNext = NULL;
                scinfo.flags = 0;
                vkCreateSemaphore(context->dev->ldev, &scinfo, NULL, &context->image_semaphores[i]);
                vkCreateSemaphore(context->dev->ldev, &scinfo, NULL, &context->queue_semaphores[i]);
                context->fences_in_flight[i] = create_vkfence(context->dev, 1);
        }
        context->images_in_flight = rune_calloc(0, sizeof(vkfence_t*) * context->swapchain->img_count);

        gettimeofday(&stop, NULL);
        log_output(LOG_INFO, "Finished initializing Vulkan in %lums", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        return 0;
}

void _close_vulkan(void) {
        vkDeviceWaitIdle(context->dev->ldev);
        for (uint8_t i = 0; i < context->swapchain->max_frames; i++) {
                if (context->image_semaphores[i] != NULL)
                        vkDestroySemaphore(context->dev->ldev, context->image_semaphores[i], NULL);
                if (context->queue_semaphores[i] != NULL)
                        vkDestroySemaphore(context->dev->ldev, context->queue_semaphores[i], NULL);
                destroy_vkfence(context->fences_in_flight[i], context->dev);
        }

        _destroy_cmdbuffers();
        _destroy_framebuffers();
        destroy_vkrendpass(context->rendpass, context->dev);
        destroy_swapchain(context->swapchain, context->dev);
        destroy_vkdev(context->dev);
        destroy_vkcontext(context);
}

int _begin_frame(float time) {
        vkfence_t *frame_fence = context->fences_in_flight[context->swapchain->frame];
        if (fence_lock(frame_fence, context->dev, UINT64_MAX) == -1) {
                log_output(LOG_WARN, "Error locking in-flight fence");
                return -1;
        }
        
        uint32_t next_img = vkswapchain_get_next_img(context->swapchain,
                                                     context->dev,
                                                     UINT64_MAX,
                                                     NULL,
                                                     context->image_semaphores[context->swapchain->frame]);

        if (next_img == -1)
                return -1;

        context->img_index = next_img;
        vkcmdbuffer_t *cmdbuf = context->cmdbuffers[context->img_index];
        cmdbuf_begin(cmdbuf, 0, 0, 0);

        VkViewport vport;
        vport.x = 0;
        vport.y = (float)context->surface->height;
        vport.width = (float)context->surface->width;
        vport.height = (float)context->surface->height;
        vport.minDepth = 0;
        vport.maxDepth = 1;

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = context->surface->width;
        scissor.extent.height = context->surface->height;

        vkCmdSetViewport(cmdbuf->handle, 0, 1, &vport);
        vkCmdSetScissor(cmdbuf->handle, 0, 1, &scissor);

        context->rendpass->area[2] = context->surface->width;
        context->rendpass->area[3] = context->surface->height;

        VkFramebuffer framebuf = context->framebuffers[context->img_index]->handle;
        renderpass_begin(cmdbuf, context->rendpass, framebuf);
        return 0;
}

int _end_frame(float time) {
        vkcmdbuffer_t *cmdbuf = context->cmdbuffers[context->img_index];
        renderpass_end(cmdbuf, context->rendpass);
        cmdbuf_end(cmdbuf);

        vkfence_t** img_in_flight = &context->images_in_flight[context->img_index];
        if (*img_in_flight != NULL)
                fence_lock(*img_in_flight, context->dev, UINT64_MAX);

        context->images_in_flight[context->img_index] = context->fences_in_flight[context->swapchain->frame];
        *img_in_flight = context->fences_in_flight[context->swapchain->frame];
        fence_unlock(*img_in_flight, context->dev);

        for (int i = 0; i < context->dev->num_gfx_queues; i++) {
                cmdbuf_submit(cmdbuf,
                              &context->queue_semaphores[context->swapchain->frame],
                              &context->image_semaphores[context->swapchain->frame],
                              context->dev->gfx_queues[i],
                              (*img_in_flight)->handle);
        }

        vkswapchain_present(context->swapchain,
                            context->dev,
                            &context->queue_semaphores[context->swapchain->frame],
                            &context->img_index);

        cmdbuf_reset(cmdbuf);

        return 0;
}

void _draw_vulkan(void) {
        _begin_frame(0);
        _end_frame(0);
}

void _clear_vulkan(void) {
}

renderer_t* select_render_vulkan(window_t *window) {
        renderer_t *ret = rune_alloc(sizeof(renderer_t));
        ret->close = _close_vulkan;
        ret->draw = _draw_vulkan;
        ret->clear = _clear_vulkan;
        if (_init_vulkan(window) != 0)
                rune_abort();
        return ret;
}
