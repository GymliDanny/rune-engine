#ifndef VKSURFACE_H
#define VKSURFACE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct vksurface {
        VkSurfaceKHR handle;
        uint32_t width;
        uint32_t height;
};

#endif
