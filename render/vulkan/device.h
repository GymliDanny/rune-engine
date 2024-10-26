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

#ifndef VKDEVICE_H
#define VKDEVICE_H

#include <rune/util/types.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct vkswapchain_data {
        VkSurfaceCapabilitiesKHR capabilities;
        VkSurfaceFormatKHR *formats;
        VkPresentModeKHR *present_modes;
        uint32_t format_count;
        uint32_t present_count;
};

struct vkdev {
        VkPhysicalDevice pdev;
        VkDevice ldev;
        struct vkswapchain_data scdata;
        uint32_t gfx_index;
        uint32_t pres_index;
        uint32_t tsfr_index;
        uint32_t comp_index;
        VkQueue gfx_queue;
        VkQueue pres_queue;
        VkQueue tsfr_queue;
        VkQueue comp_queue;
        VkCommandPool cmd_pool;
        VkFormat depth_format;
};

struct vkdev* create_vkdev(VkInstance instance, VkSurfaceKHR surface);
void destroy_vkdev(struct vkdev *dev);

void get_swapchain_data(struct vkdev *dev, VkSurfaceKHR *surface);
int get_depth_format(struct vkdev *dev);
uint32_t get_memory_index(struct vkdev *dev, uint32_t type, uint32_t flags);

#endif
