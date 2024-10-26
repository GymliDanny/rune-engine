#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <rune/render/renderer.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <rune/core/abort.h>
#include "context.h"
#include "image.h"
#include <sys/time.h>

static struct vkcontext *context = NULL;

void _init_cmdbuffers(void) {
        uint32_t num_buffers = context->swapchain->img_count;
        if (context->cmdbuffers == NULL)
                context->cmdbuffers = rune_calloc(0, sizeof(struct vkcmdbuffer*) * num_buffers);

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
                context->framebuffers = rune_calloc(0, sizeof(struct vkframebuffer*) * num_buffers);

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

int _init_vulkan(struct rune_window *window) {
        struct timeval start;
        struct timeval stop;
        gettimeofday(&start, NULL);

        struct ext_container ext;
        ext.extensions = glfwGetRequiredInstanceExtensions(&ext.ext_count);
        struct vklayer_container *vklayers = init_vklayers(&ext);

        context = create_vkcontext(vklayers, &ext);
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
                                         context->surface->handle);
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
        context->fences_in_flight = rune_calloc(0, sizeof(struct vkfence*) * context->swapchain->max_frames);

        VkSemaphoreCreateInfo scinfo;
        for (uint8_t i = 0; i < context->swapchain->max_frames; i++) {
                scinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                scinfo.pNext = NULL;
                scinfo.flags = 0;
                vkCreateSemaphore(context->dev->ldev, &scinfo, NULL, &context->image_semaphores[i]);
                vkCreateSemaphore(context->dev->ldev, &scinfo, NULL, &context->queue_semaphores[i]);
                context->fences_in_flight[i] = create_vkfence(context->dev, 1);
        }
        context->images_in_flight = rune_calloc(0, sizeof(struct vkfence*) * context->swapchain->img_count);

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

void _draw_vulkan(void) {
        if (fence_lock(context->fences_in_flight[context->swapchain->frame], context->dev, UINT64_MAX) != 0)
                log_output(LOG_WARN, "In-flight fence locking failure");
}

void _clear_vulkan(void) {
}

struct rune_renderer* select_render_vulkan(struct rune_window *window) {
        struct rune_renderer *ret = rune_alloc(sizeof(struct rune_renderer));
        ret->init = _init_vulkan;
        ret->close = _close_vulkan;
        ret->draw = _draw_vulkan;
        ret->clear = _clear_vulkan;
        if ((*ret->init)(window) != 0)
                rune_abort();
        return ret;
}
