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

#ifndef RUNE_UTIL_UTIL_H
#define RUNE_UTIL_UTIL_H

#include <assert.h>
#include <stdint.h>

#if defined(__clang__) || defined(__gcc__)
        #define STATIC_ASSERT _Static_assert
#else
        #define STATIC_ASSERT static_assert
#endif


#ifdef REXPORT
        #ifdef _MSC_VER
                #define RAPI __declspec(dllexport)
        #else
                #define RAPI __attribute__((visibility("default")))
        #endif
#else
        #ifdef _MSC_VER
                #define RAPI __declspec(dllexport)
        #else
                #define RAPI
        #endif
#endif

static inline uint32_t clamp(uint32_t value, float min, float max) {
        const uint32_t ret = value < min ? min : value;
        return ret > max ? max : ret;
}

#endif
