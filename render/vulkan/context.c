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

#include "context.h"
#include "vkassert.h"
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <rune/core/config.h>
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

int _init_vkdebugger(vkcontext_t *context) {
        log_output(LOG_INFO, "Initializing Vulkan debugger");

        VkDebugUtilsMessengerCreateInfoEXT dbinfo;
        dbinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        dbinfo.pNext = NULL;
        dbinfo.flags = 0;
        dbinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        dbinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        dbinfo.pfnUserCallback = _vulkan_db_callback;
        dbinfo.pUserData = NULL;

        PFN_vkCreateDebugUtilsMessengerEXT func =
                (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(context->instance,
                                      "vkCreateDebugUtilsMessengerEXT");

        if (func == NULL) {
                log_output(LOG_ERROR, "Failed to create Vulkan debugger");
                return -1;
        }

        VkResult res;
        res = func(context->instance, &dbinfo, NULL, &context->db_messenger);
        if (res != VK_SUCCESS) {
                char *err_str = get_vkerr_str(res);
                log_output(LOG_ERROR, "Debug session error: %s", err_str);
                free(err_str);
                return -1;
        }
        return 0;
}

uint32_t _vertoi(const char *version) {
        uint32_t major = 0;
        uint32_t minor = 0;
        uint32_t patch = 0;
        char *version_copy = strdup(version);
        char *token = strtok(version_copy, ".");
        if (token != NULL)
                major = (uint32_t)atoi(token);

        token = strtok(NULL, ".");
        if (token != NULL)
                minor = (uint32_t)atoi(token);

        token = strtok(NULL, ".");
        if (token != NULL)
                patch = (uint32_t)atoi(token);

        free(version_copy);
        return (major << 22) | (minor << 12) | patch;
}

vkcontext_t* create_vkcontext(vklayer_container_t *vklayers, ext_container_t *ext) {
        vkcontext_t *ret = rune_calloc(0, sizeof(vkcontext_t));
        ret->surface = rune_alloc(sizeof(vksurface_t));

        VkApplicationInfo app_info;
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pNext = NULL;
        app_info.apiVersion = VK_API_VERSION_1_2;
        app_info.pApplicationName = rune_get_app_name();
        const char *app_ver = rune_get_app_ver();
        app_info.applicationVersion = _vertoi(app_ver);
        app_info.pEngineName = "RuneEngine";
        app_info.engineVersion = _vertoi(RUNE_VER);

        VkInstanceCreateInfo cinfo;
        cinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        cinfo.pNext = NULL;
        cinfo.flags = 0;
        cinfo.pApplicationInfo = &app_info;
        cinfo.enabledExtensionCount = ext->ext_count;
        cinfo.ppEnabledExtensionNames = ext->extensions;
        cinfo.pNext = NULL;

        if (vklayers != NULL) {
                log_output(LOG_INFO, "Validation layers enabled");
                cinfo.enabledLayerCount = vklayers->vklayer_count;
                cinfo.ppEnabledLayerNames = vklayers->vklayer_names;
        } else {
                log_output(LOG_INFO, "Validation layers disabled");
                cinfo.enabledLayerCount = 0;
                cinfo.ppEnabledLayerNames = NULL;
        }

        VkResult res;
        res = vkCreateInstance(&cinfo, NULL, &ret->instance);
        if (res != VK_SUCCESS) {
                char *err_str = get_vkerr_str(res);
                log_output(LOG_FATAL, "Cannot create a Vulkan instance: %s", err_str);
                rune_abort();
        }
        
        if (vklayers != NULL)
                _init_vkdebugger(ret);

        return ret;
}

void destroy_vkcontext(vkcontext_t *context) {
        log_output(LOG_DEBUG, "Closing Vulkan instance");
        if (rune_get_vk_debug() == 1) {
                PFN_vkDestroyDebugUtilsMessengerEXT func =
                        (PFN_vkDestroyDebugUtilsMessengerEXT)
                        vkGetInstanceProcAddr(context->instance,
                                              "vkDestroyDebugUtilsMessengerEXT");
                func(context->instance, context->db_messenger, NULL);
        }
        vkDestroySurfaceKHR(context->instance, context->surface->handle, NULL);
        vkDestroyInstance(context->instance, NULL);
        rune_free(context->surface);
        rune_free(context);
}

vklayer_container_t* init_vklayers(ext_container_t *ext) {
        const char** new_extensions = rune_alloc(sizeof(char*) * ext->ext_count++);
        for (uint32_t i = 0; i < ext->ext_count-1; i++)
                new_extensions[i] = ext->extensions[i];
        new_extensions[ext->ext_count-1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        ext->extensions = new_extensions;

        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, NULL);
        VkLayerProperties layer_props[layer_count];
        vkEnumerateInstanceLayerProperties(&layer_count, layer_props);

        vklayer_container_t *ret = rune_alloc(sizeof(vklayer_container_t));
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
