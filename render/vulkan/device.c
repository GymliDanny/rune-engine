#include "device.h"
#include "vkassert.h"
#include <rune/core/alloc.h>
#include <rune/core/logging.h>

struct vkqfam_data {
        uint32_t gfx;
        uint32_t present;
        uint32_t compute;
        uint32_t transfer;
};

struct vkdev_data {
        VkPhysicalDeviceProperties pdev_props;
        VkPhysicalDeviceFeatures pdev_feats;
        VkPhysicalDeviceMemoryProperties pdev_mprops;
        uint32_t pdev_ext_count;
        const char** pdev_extensions;
};

int _query_qfam_data(VkSurfaceKHR surface, VkPhysicalDevice pdev, VkQueueFamilyProperties **qfam_props) {
        uint32_t qfam_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pdev, &qfam_count, NULL);
        *qfam_props = rune_alloc(sizeof(VkQueueFamilyProperties) * qfam_count);
        vkGetPhysicalDeviceQueueFamilyProperties(pdev, &qfam_count, *qfam_props);
        return qfam_count;
}

void _query_pdev_data(VkPhysicalDevice pdev, struct vkdev_data *pdata) {
        vkGetPhysicalDeviceProperties(pdev, &pdata->pdev_props);
        vkGetPhysicalDeviceFeatures(pdev, &pdata->pdev_feats);
        vkGetPhysicalDeviceMemoryProperties(pdev, &pdata->pdev_mprops);
}

int _get_qfam_index(int num_props, uint32_t queue_type, VkQueueFamilyProperties *qfam_props) {
        for (int i = 0; i < num_props; i++) {
                if (qfam_props[i].queueFlags & queue_type)
                        return i;
        }
        return -1;
}

int _check_pdev(VkSurfaceKHR surface, VkPhysicalDevice pdev) {
        int score = 0;

        struct vkdev_data pdata;
        _query_pdev_data(pdev, &pdata);
        if (pdata.pdev_props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return score;

        VkQueueFamilyProperties *qfam_props;
        int num_qfams = _query_qfam_data(surface, pdev, &qfam_props);
        if (_get_qfam_index(num_qfams, VK_QUEUE_GRAPHICS_BIT, qfam_props) != -1)
                score += 20;
        if (_get_qfam_index(num_qfams, VK_QUEUE_COMPUTE_BIT, qfam_props) != -1)
                score += 5;
        if (_get_qfam_index(num_qfams, VK_QUEUE_TRANSFER_BIT, qfam_props) != -1)
                score += 20;

        VkBool32 present_bit;
        vkGetPhysicalDeviceSurfaceSupportKHR(pdev, 0, surface, &present_bit);
        if (present_bit != VK_FALSE)
                score += 20;

        rune_free(qfam_props);
        return score;
}

struct vkdev* create_vkdev(VkInstance instance, VkSurfaceKHR surface) {
        uint32_t pdev_count;
        vkEnumeratePhysicalDevices(instance, &pdev_count, NULL);
        if (pdev_count == 0)
                return NULL;
        VkPhysicalDevice pdevs[pdev_count];
        vkEnumeratePhysicalDevices(instance, &pdev_count, pdevs);

        uint32_t selected_pdev = -1;
        for (uint32_t i = 0; i < pdev_count; i++) {
                if (_check_pdev(surface, pdevs[i]) >= 60) {
                        selected_pdev = i;
                        break;
                }
        }

        if (selected_pdev == -1) {
                log_output(LOG_FATAL, "No device meets minimum requirements for rendering");
                return NULL;
        }

        struct vkdev *dev = rune_alloc(sizeof(struct vkdev));
        dev->pdev = pdevs[selected_pdev];

        VkQueueFamilyProperties *qfam_props;
        int num_qfams = _query_qfam_data(surface, pdevs[selected_pdev], &qfam_props);
        VkBool32 present_support;
        for (int i = 0; i < num_qfams; i++) {
                vkassert(vkGetPhysicalDeviceSurfaceSupportKHR(dev->pdev, i, surface, &present_support), "Error retrieving present queue support");
                if (present_support == VK_TRUE)
                        dev->pres_index = i;
        }
        dev->gfx_index = _get_qfam_index(num_qfams, VK_QUEUE_GRAPHICS_BIT, qfam_props);
        dev->tsfr_index = _get_qfam_index(num_qfams, VK_QUEUE_TRANSFER_BIT, qfam_props);
        dev->comp_index = _get_qfam_index(num_qfams, VK_QUEUE_COMPUTE_BIT, qfam_props);

        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo qcinfos[3];
        qcinfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qcinfos[0].queueFamilyIndex = dev->gfx_index;
        qcinfos[0].queueCount = 1;
        qcinfos[0].flags = 0;
        qcinfos[0].pNext = NULL;
        qcinfos[0].pQueuePriorities = &queue_priority;

        qcinfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qcinfos[1].queueFamilyIndex = dev->tsfr_index;
        qcinfos[1].queueCount = 1;
        qcinfos[1].flags = 0;
        qcinfos[1].pNext = NULL;
        qcinfos[1].pQueuePriorities = &queue_priority;

        qcinfos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qcinfos[2].queueFamilyIndex = dev->comp_index;
        qcinfos[2].queueCount = 1;
        qcinfos[2].flags = 0;
        qcinfos[2].pNext = NULL;
        qcinfos[2].pQueuePriorities = &queue_priority;

        struct vkdev_data pdata;
        _query_pdev_data(pdevs[selected_pdev], &pdata);

        VkDeviceCreateInfo dcinfo;
        dcinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        dcinfo.pNext = NULL;
        dcinfo.flags = 0;
        dcinfo.queueCreateInfoCount = 2;
        dcinfo.pQueueCreateInfos = qcinfos;
        dcinfo.enabledLayerCount = 0;
        dcinfo.ppEnabledLayerNames = NULL;
        dcinfo.enabledExtensionCount = 1;
        const char *ext_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        dcinfo.ppEnabledExtensionNames = &ext_names;
        dcinfo.pEnabledFeatures = &pdata.pdev_feats;
        vkassert(vkCreateDevice(dev->pdev, &dcinfo, NULL, &dev->ldev), "Error creating Vulkan logical device");

        vkGetDeviceQueue(dev->ldev, dev->gfx_index, 0, &dev->gfx_queue);
        vkGetDeviceQueue(dev->ldev, dev->tsfr_index, 0, &dev->tsfr_queue);
        vkGetDeviceQueue(dev->ldev, dev->pres_index, 0, &dev->pres_queue);
        vkGetDeviceQueue(dev->ldev, dev->comp_index, 0, &dev->comp_queue);

        VkCommandPoolCreateInfo pcinfo;
        pcinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pcinfo.pNext = NULL;
        pcinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pcinfo.queueFamilyIndex = dev->gfx_index;
        vkassert(vkCreateCommandPool(dev->ldev, &pcinfo, NULL, &dev->cmd_pool), "Error creating Vulkan command pool");
        
        log_output(LOG_DEBUG, "Initialized new logical device");
        return dev;
}

void destroy_vkdev(struct vkdev *dev) {
        vkDestroyDevice(dev->ldev, NULL);
        rune_free(dev);
}

void get_swapchain_data(struct vkdev *dev, VkSurfaceKHR *surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev->pdev, *surface, &dev->scdata.capabilities);

        vkGetPhysicalDeviceSurfaceFormatsKHR(dev->pdev, *surface, &dev->scdata.format_count, NULL);
        dev->scdata.formats = rune_alloc(sizeof(VkSurfaceFormatKHR) * dev->scdata.format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev->pdev, *surface, &dev->scdata.format_count, dev->scdata.formats);

        vkGetPhysicalDeviceSurfacePresentModesKHR(dev->pdev, *surface, &dev->scdata.present_count, NULL);
        dev->scdata.present_modes = rune_alloc(sizeof(VkPresentModeKHR) * dev->scdata.present_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(dev->pdev, *surface, &dev->scdata.present_count, dev->scdata.present_modes);
}

int get_depth_format(struct vkdev *dev) {
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

uint32_t get_memory_index(struct vkdev *dev, uint32_t type, uint32_t flags) {
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
