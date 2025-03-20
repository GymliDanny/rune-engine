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

#include "device.h"
#include "vkassert.h"
#include <rune/core/alloc.h>
#include <rune/core/logging.h>

static int gfx_qfam = -1;
static int tsfr_qfam = -1;
static int comp_qfam = -1;
static int pres_qfam = -1;

struct vkdev_data {
        VkPhysicalDeviceProperties pdev_props;
        VkPhysicalDeviceFeatures pdev_feats;
        VkPhysicalDeviceMemoryProperties pdev_mprops;
        uint32_t pdev_ext_count;
        const char** pdev_extensions;
};

void _query_pdev_data(VkPhysicalDevice pdev, struct vkdev_data *pdata) {
        vkGetPhysicalDeviceProperties(pdev, &pdata->pdev_props);
        vkGetPhysicalDeviceFeatures(pdev, &pdata->pdev_feats);
        vkGetPhysicalDeviceMemoryProperties(pdev, &pdata->pdev_mprops);
}

uint32_t _query_qfam_data(VkSurfaceKHR surface, VkPhysicalDevice pdev, VkQueueFamilyProperties** qfam_props) {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, NULL);
        *qfam_props = rune_alloc(sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, *qfam_props);
        return count;
}

int _query_gfx_index(int num_props, VkQueueFamilyProperties *qfam_props) {
        for (int i = 0; i < num_props; i++) {
                if (qfam_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        gfx_qfam = i;
        }
        return gfx_qfam;
}

int _query_tsfr_index(int num_props, VkQueueFamilyProperties *qfam_props) {
        for (int i = 0; i < num_props; i++) {
                if (qfam_props[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                        tsfr_qfam = i;
        }
        return tsfr_qfam;
}

int _query_comp_index(int num_props, VkQueueFamilyProperties *qfam_props) {
        for (int i = 0; i < num_props; i++) {
                if (qfam_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                        comp_qfam = i;
        }
        return comp_qfam;
}

int _query_pres_index(int num_props, VkQueueFamilyProperties *qfam_props, VkPhysicalDevice pdev, VkSurfaceKHR surface) {
        VkBool32 present_bit;
        for (int i = 0; i < num_props; i++) {
                vkGetPhysicalDeviceSurfaceSupportKHR(pdev, 0, surface, &present_bit);
                if (present_bit != VK_FALSE)
                        pres_qfam = i;
        }
        return pres_qfam;
}

int _check_pdev(VkSurfaceKHR surface, VkPhysicalDevice pdev) {
        int score = 0;

        struct vkdev_data pdata;
        _query_pdev_data(pdev, &pdata);
        if (pdata.pdev_props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return score;

        VkQueueFamilyProperties *qfam_props;
        uint32_t num_qfams = _query_qfam_data(surface, pdev, &qfam_props); 
        if (_query_gfx_index(num_qfams, qfam_props) != -1)
                score += 20;
        if (_query_comp_index(num_qfams, qfam_props) != -1)
                score += 20;
        if (_query_tsfr_index(num_qfams, qfam_props) != -1)
                score += 20;
        if (_query_pres_index(num_qfams, qfam_props, pdev, surface) != -1)
                score += 20;

        rune_free(qfam_props);
        return score;
}

VkPhysicalDevice _select_pdev(VkInstance instance, VkSurfaceKHR surface) {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance, &count, NULL);
        if (count == 0)
                return NULL;

        VkPhysicalDevice pdevs[count];
        vkEnumeratePhysicalDevices(instance, &count, pdevs);

        for (uint32_t i = 0; i < count; i++) {
                if (_check_pdev(surface, pdevs[i]) >= 80)
                        return pdevs[i];
        }
        return NULL;
}

void _create_queue(vkdev_t *dev, int qfam_index, int queue_index) {
        vkGetDeviceQueue(dev->ldev, qfam_index, queue_index, &dev->queues[qfam_index].handle);
        if (dev->queues[qfam_index].handle == NULL) {
                log_output(LOG_FATAL, "Error creating required Vulkan queue");
                rune_abort();
        }
}

vkdev_t* create_vkdev(VkInstance instance, VkSurfaceKHR surface) {
        VkPhysicalDevice pdev = _select_pdev(instance, surface);
        if (pdev == NULL) {
                log_output(LOG_FATAL, "No device meets minimum requirements for rendering");
                rune_abort();
        }

        vkdev_t *dev = rune_calloc(0, sizeof(vkdev_t));
        dev->pdev = pdev;

        dev->queues[0].qfam = gfx_qfam;
        dev->queues[1].qfam = tsfr_qfam;
        dev->queues[2].qfam = comp_qfam;
        dev->queues[3].qfam = 0;

        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo qcinfos[3];
        for (int i = 0; i < 3; i++) {
                qcinfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                qcinfos[i].pNext = NULL;
                qcinfos[i].flags = 0;
                qcinfos[i].queueFamilyIndex = dev->queues[i].qfam;
                qcinfos[i].queueCount = 1;
                qcinfos[i].pQueuePriorities = &queue_priority;
        }

        struct vkdev_data pdata;
        _query_pdev_data(pdev, &pdata);

        VkDeviceCreateInfo dcinfo;
        dcinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        dcinfo.pNext = NULL;
        dcinfo.flags = 0;
        dcinfo.queueCreateInfoCount = 3;
        dcinfo.pQueueCreateInfos = qcinfos;
        dcinfo.enabledLayerCount = 0;
        dcinfo.ppEnabledLayerNames = NULL;
        dcinfo.enabledExtensionCount = 1;
        const char *ext_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        dcinfo.ppEnabledExtensionNames = &ext_names;
        dcinfo.pEnabledFeatures = &pdata.pdev_feats;
        vkassert(vkCreateDevice(dev->pdev, &dcinfo, NULL, &dev->ldev));

        for (uint32_t i = 0; i < 3; i++)
                _create_queue(dev, i, 0);

        // FIXME: This is a dirty hack and should be fixed
        dev->queues[3].handle = dev->queues[0].handle;

        VkCommandPoolCreateInfo pcinfo;
        pcinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pcinfo.pNext = NULL;
        pcinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pcinfo.queueFamilyIndex = dev->queues[0].qfam;
        vkassert(vkCreateCommandPool(dev->ldev, &pcinfo, NULL, &dev->cmd_pool));
        
        log_output(LOG_DEBUG, "Initialized new logical device");
        return dev;
}

void destroy_vkdev(vkdev_t *dev) {
        vkDestroyCommandPool(dev->ldev, dev->cmd_pool, NULL);
        vkDestroyDevice(dev->ldev, NULL);
        rune_free(dev);
}

void get_swapchain_data(vkdev_t *dev, VkSurfaceKHR *surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev->pdev, *surface, &dev->scdata.capabilities);

        vkGetPhysicalDeviceSurfaceFormatsKHR(dev->pdev, *surface, &dev->scdata.format_count, NULL);
        dev->scdata.formats = rune_alloc(sizeof(VkSurfaceFormatKHR) * dev->scdata.format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev->pdev, *surface, &dev->scdata.format_count, dev->scdata.formats);

        vkGetPhysicalDeviceSurfacePresentModesKHR(dev->pdev, *surface, &dev->scdata.present_count, NULL);
        dev->scdata.present_modes = rune_alloc(sizeof(VkPresentModeKHR) * dev->scdata.present_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(dev->pdev, *surface, &dev->scdata.present_count, dev->scdata.present_modes);
}

int get_depth_format(vkdev_t *dev) {
        const uint64_t count = 3;
        VkFormat formats[3] = {
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D32_SFLOAT};
        
        uint32_t flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        VkFormatProperties props;
        for (uint64_t i = 0; i < count; i++) {
                vkGetPhysicalDeviceFormatProperties(dev->pdev, formats[i], &props);
                if ((props.linearTilingFeatures & flags) == flags) {
                        dev->depth_format = formats[i];
                        return 1;
                } else if ((props.optimalTilingFeatures & flags) == flags) {
                        dev->depth_format = formats[i];
                        return 1;
                }
        }
        return 0;
}

uint32_t get_memory_index(vkdev_t *dev, uint32_t type, uint32_t flags) {
        VkPhysicalDeviceMemoryProperties mem_props;
        vkGetPhysicalDeviceMemoryProperties(dev->pdev, &mem_props);

        for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
                uint32_t tmp = type & (1 << i);
                uint32_t prop_flags = mem_props.memoryTypes[i].propertyFlags;
                if (tmp & prop_flags & flags)
                        return i;
        }

        log_output(LOG_WARN, "Unable to find suitable memory type");
        return -1;
}
