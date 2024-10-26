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

#ifndef RUNE_ALLOC_H
#define RUNE_ALLOC_H

#include <rune/util/types.h>
#include <rune/util/list.h>

/**
 * Memory block used for memory accounting
 */
struct mem_block {
        void *ptr;
        size_t sz;
        int free;
        struct list_head list;
};

/**
 * \brief Custom malloc implementation
 * \param[in] sz The size of the requested memory block
 * \return A pointer to void, or NULL in case of error
 */
RAPI void* rune_alloc(size_t sz);

/**
 * \brief Custom calloc implementation
 * \param[in] nmemb An integer to fill the memory block with
 * \param[in] sz The size of the requested memory block
 * \return A pointer to void, or NULL in case of error
 */
RAPI void* rune_calloc(size_t nmemb, size_t sz);

/**
 * \brief Custom realloc implementation
 * \param[in] ptr A void pointer to a previously alloc'd block, or NULL
 * \param[in] sz The size of the requested memory block
 * \return A pointer to void, or NULL in case of error
 */
RAPI void* rune_realloc(void *ptr, size_t sz);

/**
 * \brief Custom free implementation
 * \param[in] ptr A void pointer to a previously alloc'd block
 */
RAPI void rune_free(void *ptr);

/**
 * \brief Used to free all memory currently in use by the engine
 * This function should only be used at the end of execution, and is normally
 * called by rune_exit.
 */
RAPI void rune_free_all(void);

#endif
