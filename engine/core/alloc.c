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

#include <rune/core/alloc.h>
#include <rune/core/logging.h>
#include <rune/core/profiling.h>
#include <stdlib.h>
#include <string.h>

// TODO: implement block coalescing so we can reuse freed blocks

#define DEADBLOCK       ((void*)0xDEADBEEF)

static mem_block_t first_block;

static mem_block_t* _find_free_block(size_t sz) {
        list_head_t *temp = &first_block.list;
        mem_block_t *block;
        while (temp != NULL) {
                block = (mem_block_t*)((void*)temp - offsetof(mem_block_t, list));
                if (block->sz == sz && block->free == 1)
                        return block;
                temp = temp->next;
        }
        return NULL;
}

static mem_block_t* _find_block(void *ptr) {
        list_head_t *temp = &first_block.list;
        mem_block_t *block;
        while (temp != NULL) {
                block = (mem_block_t*)((void*)temp - offsetof(mem_block_t, list));
                if (block->ptr == ptr)
                        return block;
                temp = temp->next;
        }
        return NULL;
}

static mem_block_t* _alloc_block(size_t sz) {
        RUNE_PROFILE_SCOPE("Block allocation");
        if (first_block.ptr == NULL) {
                first_block.ptr = DEADBLOCK;
                first_block.sz = 0;
        }

        mem_block_t *ret = _find_free_block(sz);
        if (ret != NULL) {
                ret->free = 0;
                RUNE_PROFILE_END();
                return ret->ptr;
        }

        ret = malloc(sizeof(mem_block_t));
        if (ret == NULL) {
                RUNE_PROFILE_END();
                log_output(LOG_ERROR, "Cannot allocate block of size %d", sz);
                return NULL;
        }

        ret->ptr = malloc(sz);
        ret->sz = sz;
        ret->free = 0;
        list_add(&ret->list, &first_block.list);
        RUNE_PROFILE_END();
        log_output(LOG_DEBUG, "Alloc'd block of size %d", sz);
        return ret;
}

static void _free_block(mem_block_t *block, int hard) {
        RUNE_PROFILE_SCOPE("Block free");
        if (hard == 1) {
                list_del(&block->list);
                RUNE_PROFILE_END();
                free(block);
                log_output(LOG_DEBUG, "Freed block of size %d", block->sz);
                return;
        }
        block->free = 1;
}

void* rune_alloc(size_t sz) {
        if (sz == 0)
                return NULL;

        RUNE_PROFILE_SCOPE("Pool allocation");
        mem_block_t *block = _find_free_block(sz);
        if (block != NULL) {
                block->free = 0;
                RUNE_PROFILE_END();
                return block->ptr;
        }

        block = _alloc_block(sz);
        RUNE_PROFILE_END();
        return block->ptr;
}

void* rune_calloc(size_t nmemb, size_t sz) {
        if (sz == 0)
                return NULL;

        RUNE_PROFILE_SCOPE("Zero array pool allocation");
        mem_block_t *block = _find_free_block(sz);
        if (block != NULL) {
                block->free = 0;
                RUNE_PROFILE_END();
                return block->ptr;
        }

        block = _alloc_block(sz);
        memset(block->ptr, 0, sz);
        RUNE_PROFILE_END();
        return block->ptr;
}

void* rune_realloc(void *ptr, size_t sz) {
        if (ptr == NULL || sz == 0)
                return rune_alloc(sz);

        RUNE_PROFILE_SCOPE("Pool reallocation");
        mem_block_t *old = _find_block(ptr);
        mem_block_t *new = _alloc_block(sz);
        memcpy(new->ptr, old->ptr, old->sz);
        old->free = 1;
        RUNE_PROFILE_END();
        return new->ptr;
}

void rune_free(void *ptr) {
        if (ptr == NULL)
                return;

        RUNE_PROFILE_SCOPE("Pool free");
        mem_block_t *block = _find_block(ptr);
        if (block->free == 1) {
                RUNE_PROFILE_END();
                return;
        }
        block->free = 1;
        RUNE_PROFILE_END();
}

void rune_free_all(void) {
        RUNE_PROFILE_SCOPE("Pool free all");
        list_head_t *temp = &first_block.list;
        mem_block_t *block;
        while (temp != NULL) {
                block = (mem_block_t*)((void*)temp - offsetof(mem_block_t, list));
                if (block->ptr == DEADBLOCK) {
                        temp = temp->next;
                        continue;
                }

                temp = temp->next;
                if (block->ptr != NULL)
                        _free_block(block, 1);
        }
        RUNE_PROFILE_END();
}
