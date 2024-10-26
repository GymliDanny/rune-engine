#ifndef VKCONTEXT_H
#define VKCONTEXT_H

#include "surface.h"
#include "device.h"
#include "swapchain.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "fence.h"

struct ext_container {
        const char** extensions;
        uint32_t ext_count;
};

struct vklayer_container {
        const char** vklayer_names;
        uint32_t vklayer_count;
};

struct vkcontext {
        VkInstance instance;
        VkDebugUtilsMessengerEXT db_messenger;
        VkSemaphore *queue_semaphores;
        VkSemaphore *image_semaphores;
        struct vksurface *surface;
        struct vkswapchain *swapchain;
        struct vkrendpass *rendpass;
        struct vkdev *dev;
        struct vkcmdbuffer** cmdbuffers;
        struct vkframebuffer** framebuffers;
        struct vkfence** fences_in_flight;
        struct vkfence** images_in_flight;
        uint32_t num_fences_in_flight;
        uint32_t img_index;
};

struct vkcontext* create_vkcontext(struct vklayer_container *vklayers, struct ext_container *ext);
void destroy_vkcontext(struct vkcontext *context);

struct vklayer_container* init_vklayers(struct ext_container *ext);

#endif
