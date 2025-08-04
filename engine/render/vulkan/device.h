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

#include "vk_types.h"

vkdev_t* create_vkdev(VkInstance instance, VkSurfaceKHR surface, int num_gfx, int num_tsfr, int num_comp, int presentable);
void destroy_vkdev(vkdev_t *dev);

void get_swapchain_data(vkdev_t *dev, VkSurfaceKHR *surface);
int get_depth_format(vkdev_t *dev);
uint32_t get_memory_index(vkdev_t *dev, uint32_t type, uint32_t flags);

#endif
