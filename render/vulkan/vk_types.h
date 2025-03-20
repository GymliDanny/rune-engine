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

#ifndef VK_TYPES_H
#define VK_TYPES_H

#include <rune/util/types.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

typedef struct vkqueue {
        uint32_t qfam;
        VkQueue handle;
} vkqueue_t;

typedef struct vksurface {
        VkSurfaceKHR handle;
        uint32_t width;
        uint32_t height;
} vksurface_t;

typedef struct vkcmdbuffer {
        VkCommandBuffer handle;
        int state;
} vkcmdbuffer_t;

typedef struct vkfence {
        VkFence handle;
        int signal;
} vkfence_t;

typedef struct vkimage {
        VkImage handle;
        VkDeviceMemory memory;
        VkImageView view;
        uint32_t width;
        uint32_t height;
} vkimage_t;

typedef struct ext_container {
        const char** extensions;
        uint32_t ext_count;
} ext_container_t;

typedef struct vklayer_container {
        const char** vklayer_names;
        uint32_t vklayer_count;
} vklayer_container_t;

typedef struct vkswapchain_data {
        VkSurfaceCapabilitiesKHR capabilities;
        VkSurfaceFormatKHR *formats;
        VkPresentModeKHR *present_modes;
        uint32_t format_count;
        uint32_t present_count;
} vkswapchain_data_t;

typedef struct vkrendpass {
        VkRenderPass handle;
        vec4 area;
        vec4 color;
        float depth;
        uint32_t stencil;
} vkrendpass_t;

typedef struct vkframebuffer {
        VkFramebuffer handle;
        uint32_t at_count;
        VkImageView *attachments;
        vkrendpass_t *rendpass;
} vkframebuffer_t;

typedef struct vkdev {
        VkPhysicalDevice pdev;
        VkDevice ldev;
        vkswapchain_data_t scdata;
        vkqueue_t queues[4];
        VkCommandPool cmd_pool;
        VkFormat depth_format;
} vkdev_t;

typedef struct vkswapchain {
        VkSwapchainKHR handle;
        VkSurfaceFormatKHR format_khr;
        VkFormat format;
        VkImage *images;
        VkImageView *views;
        vkimage_t *depth_attachment;
        uint8_t max_frames;
        uint32_t frame;
        uint32_t img_count;
} vkswapchain_t;

typedef struct vkcontext {
        VkInstance instance;
        VkDebugUtilsMessengerEXT db_messenger;
        VkSemaphore *queue_semaphores;
        VkSemaphore *image_semaphores;
        vksurface_t *surface;
        vkswapchain_t *swapchain;
        vkrendpass_t *rendpass;
        vkdev_t *dev;
        vkcmdbuffer_t** cmdbuffers;
        vkframebuffer_t** framebuffers;
        vkfence_t** fences_in_flight;
        vkfence_t** images_in_flight;
        uint32_t num_fences_in_flight;
        uint32_t img_index;
} vkcontext_t;

#endif
