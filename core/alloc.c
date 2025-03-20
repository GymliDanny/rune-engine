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
        if (first_block.ptr == NULL) {
                first_block.ptr == DEADBLOCK;
                first_block.sz = 0;
        }

        mem_block_t *ret = _find_free_block(sz);
        if (ret != NULL) {
                ret->free = 0;
                return ret->ptr;
        }

        ret = malloc(sizeof(mem_block_t));
        if (ret == NULL) {
                log_output(LOG_ERROR, "Cannot allocate block of size %d", sz);
                return NULL;
        }

        ret->ptr = malloc(sz);
        ret->sz = sz;
        ret->free = 0;
        list_add(&ret->list, &first_block.list);
        log_output(LOG_DEBUG, "Alloc'd block of size %d", sz);
        return ret;
}

static void _free_block(mem_block_t *block, int hard) {
        if (hard == 1) {
                list_del(&block->list);
                log_output(LOG_DEBUG, "Freed block of size %d", block->sz);
                free(block);
                return;
        }
        block->free = 1;
}

void* rune_alloc(size_t sz) {
        if (sz == 0)
                return NULL;

        mem_block_t *block = _find_free_block(sz);
        if (block != NULL) {
                block->free = 0;
                return block->ptr;
        }

        block = _alloc_block(sz);
        return block->ptr;
}

void* rune_calloc(size_t nmemb, size_t sz) {
        if (sz == 0)
                return NULL;

        mem_block_t *block = _find_free_block(sz);
        if (block != NULL) {
                block->free = 0;
                return block->ptr;
        }

        block = _alloc_block(sz);
        memset(block->ptr, 0, sz);
        return block->ptr;
}

void* rune_realloc(void *ptr, size_t sz) {
        if (ptr == NULL || sz == 0)
                return rune_alloc(sz);

        mem_block_t *old = _find_block(ptr);
        mem_block_t *new = _alloc_block(sz);
        memcpy(new->ptr, old->ptr, old->sz);
        old->free = 1;
        return new->ptr;
}

void rune_free(void *ptr) {
        if (ptr == NULL)
                return;

        mem_block_t *block = _find_block(ptr);
        if (block->free == 1)
                return;
        block->free = 1;
}

void rune_free_all(void) {
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
}
