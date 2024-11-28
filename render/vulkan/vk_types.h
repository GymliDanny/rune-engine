#ifndef VK_TYPES_H
#define VK_TYPES_H

#include <rune/util/types.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

struct vkqueue {
        uint32_t index;
        uint32_t qfam;
        VkQueue handle;
};

struct vksurface {
        VkSurfaceKHR handle;
        uint32_t width;
        uint32_t height;
};

struct vkcmdbuffer {
        VkCommandBuffer handle;
        int state;
};

struct vkfence {
        VkFence handle;
        int signal;
};

struct vkimage {
        VkImage handle;
        VkDeviceMemory memory;
        VkImageView view;
        uint32_t width;
        uint32_t height;
};

struct ext_container {
        const char** extensions;
        uint32_t ext_count;
};

struct vklayer_container {
        const char** vklayer_names;
        uint32_t vklayer_count;
};

struct vkswapchain_data {
        VkSurfaceCapabilitiesKHR capabilities;
        VkSurfaceFormatKHR *formats;
        VkPresentModeKHR *present_modes;
        uint32_t format_count;
        uint32_t present_count;
};

struct vkrendpass {
        VkRenderPass handle;
        vec4 area;
        vec4 color;
        float depth;
        uint32_t stencil;
};

struct vkframebuffer {
        VkFramebuffer handle;
        uint32_t at_count;
        VkImageView *attachments;
        struct vkrendpass *rendpass;
};

struct vkdev {
        VkPhysicalDevice pdev;
        VkDevice ldev;
        struct vkswapchain_data scdata;
        struct vkqueue queues[4];
        VkCommandPool cmd_pool;
        VkFormat depth_format;
};

struct vkswapchain {
        VkSwapchainKHR handle;
        VkSurfaceFormatKHR format_khr;
        VkFormat format;
        VkSemaphore render_complete;
        VkImage *images;
        VkImageView *views;
        struct vkimage *depth_attachment;
        uint8_t max_frames;
        uint32_t frame;
        uint32_t img_count;
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

#endif
