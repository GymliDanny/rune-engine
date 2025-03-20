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

#ifndef VKSWAPCHAIN_H
#define VKSWAPCHAIN_H

#include "vk_types.h"

vkswapchain_t* create_swapchain(vksurface_t *surface, vkdev_t *dev);
void destroy_swapchain(vkswapchain_t *swapchain, vkdev_t *dev);

int32_t vkswapchain_get_next_img(vkswapchain_t *swapchain, vkdev_t *dev, uint64_t tmout, VkFence fence, VkSemaphore img_available);
void vkswapchain_present(vkswapchain_t *swapchain, vkdev_t *dev, VkSemaphore *render_complete, uint32_t *img_index);

#endif
