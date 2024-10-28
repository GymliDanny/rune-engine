#include "context.h"
#include "vkassert.h"
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <stdlib.h>
#include <string.h>

VKAPI_ATTR VkBool32 VKAPI_CALL _vulkan_db_callback(
                VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                VkDebugUtilsMessageTypeFlagsEXT message_types,
                const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                void *user_data) {
        switch (message_severity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        log_output(LOG_ERROR, callback_data->pMessage);
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        log_output(LOG_WARN, callback_data->pMessage);
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                        log_output(LOG_INFO, callback_data->pMessage);
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                        log_output(LOG_DEBUG, callback_data->pMessage);
                        break;
                default:
                        break;
        }
        return VK_FALSE;
}

int _init_vkdebugger(struct vkcontext *context) {
        log_output(LOG_INFO, "Initializing Vulkan debugger");
        uint32_t log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        VkDebugUtilsMessengerCreateInfoEXT dbinfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        dbinfo.messageSeverity = log_severity;
        dbinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        dbinfo.pfnUserCallback = _vulkan_db_callback;
        dbinfo.pUserData = NULL;
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkCreateDebugUtilsMessengerEXT");
        if (func == NULL) {
                log_output(LOG_ERROR, "Failed to create Vulkan debugger");
                return -1;
        }

        VkResult res;
        res = func(context->instance, &dbinfo, NULL, &context->db_messenger);
        if (res != VK_SUCCESS) {
                char *err_str = get_vkerr_str(res);
                log_output(LOG_ERROR, "Cannot create a Vulkan debug session: %s", err_str);
                free(err_str);
                return -1;
        }
        return 0;
}

struct vkcontext* create_vkcontext(struct vklayer_container *vklayers, struct ext_container *ext) {
        log_output(LOG_DEBUG, "Initializing Vulkan");
        VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.apiVersion = VK_API_VERSION_1_2;
        app_info.pApplicationName = "RuneClient";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "RuneEngine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        VkInstanceCreateInfo cinfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        cinfo.pApplicationInfo = &app_info;
        cinfo.enabledExtensionCount = ext->ext_count;
        cinfo.ppEnabledExtensionNames = ext->extensions;
        cinfo.pNext = NULL;

        struct vkcontext *ret = rune_calloc(0, sizeof(struct vkcontext));
        ret->surface = rune_alloc(sizeof(struct vksurface));
        VkResult res;
        res = vkCreateInstance(&cinfo, NULL, &ret->instance);
        if (res != VK_SUCCESS) {
                char *err_str = get_vkerr_str(res);
                log_output(LOG_FATAL, "Cannot create a Vulkan instance: %s", err_str);
                free(err_str);
                rune_free(ret);
                ret = NULL;
        }

        if (vklayers != NULL) {
                log_output(LOG_INFO, "Validation layers enabled");
                _init_vkdebugger(ret);
                cinfo.enabledLayerCount = vklayers->vklayer_count;
                cinfo.ppEnabledLayerNames = vklayers->vklayer_names;
        } else {
                log_output(LOG_INFO, "Validation layers disabled");
        }

        return ret;
}

void destroy_vkcontext(struct vkcontext *context) {
        log_output(LOG_DEBUG, "Closing Vulkan instance");
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context->instance, context->db_messenger, NULL);
        vkDestroySurfaceKHR(context->instance, context->surface->handle, NULL);
        vkDestroyInstance(context->instance, NULL);
        rune_free(context->surface);
        rune_free(context);
}

struct vklayer_container* init_vklayers(struct ext_container *ext) {
        const char** new_extensions = rune_alloc(sizeof(char*) * ext->ext_count++);
        for (uint32_t i = 0; i < ext->ext_count-1; i++)
                new_extensions[i] = ext->extensions[i];
        new_extensions[ext->ext_count-1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        ext->extensions = new_extensions;

        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, NULL);
        VkLayerProperties layer_props[layer_count];
        vkEnumerateInstanceLayerProperties(&layer_count, layer_props);

        struct vklayer_container *ret = rune_alloc(sizeof(struct vklayer_container));
        ret->vklayer_count = 1;
        ret->vklayer_names = rune_alloc(sizeof(char*) * ret->vklayer_count);
        ret->vklayer_names[0] = "VK_LAYER_KHRONOS_validation";

        for (uint32_t i = 0; i < ret->vklayer_count; i++) {
                log_output(LOG_DEBUG, "Searching for layer: %s", ret->vklayer_names[i]);
                int found = 0;
                for (uint32_t j = 0; j < layer_count; j++) {
                        if (strcmp(ret->vklayer_names[i], layer_props[j].layerName) == 0) {
                                found = 1;
                                break;
                        }
                }
                if (found == 0) {
                        log_output(LOG_WARN, "Required validation layer is missing: %s", ret->vklayer_names[i]);
                        rune_free(ret);
                        rune_free(ext->extensions);
                        return NULL;
                }
        }
        log_output(LOG_INFO, "All prerequisite validation layers found");
        rune_free(ext->extensions);
        return ret;
}
