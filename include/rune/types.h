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

#ifndef RUNE_TYPES_H
#define RUNE_TYPES_H

#include <rune/util.h>

STATIC_ASSERT(sizeof(uint8_t) == 1, "Expected uint8_t to be 1 byte");
STATIC_ASSERT(sizeof(uint16_t) == 2, "Expected uint16_t to be 2 bytes");
STATIC_ASSERT(sizeof(uint32_t) == 4, "Expected uint32_t to be 4 bytes");
STATIC_ASSERT(sizeof(uint64_t) == 8, "Expected uint64_t to be 8 bytes");

STATIC_ASSERT(sizeof(int8_t) == 1, "Expected int8_t to be 1 byte");
STATIC_ASSERT(sizeof(int16_t) == 2, "Expected int16_t to be 2 bytes");
STATIC_ASSERT(sizeof(int32_t) == 4, "Expected int32_t to be 4 bytes");
STATIC_ASSERT(sizeof(int64_t) == 8, "Expected int64_t to be 8 bytes");

STATIC_ASSERT(sizeof(float) == 4, "Expected float to be 4 bytes");
STATIC_ASSERT(sizeof(double) == 8, "Expected double to be 8 bytes");

#endif
