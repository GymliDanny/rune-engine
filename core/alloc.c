#include <rune/core/alloc.h>
#include <rune/core/logging.h>
#include <stdlib.h>
#include <string.h>

// TODO: implement block coalescing so we can reuse freed blocks

#define DEADBLOCK       ((void*)0xDEADBEEF)

static struct mem_block first_block;

static struct mem_block* _find_free_block(size_t sz) {
        struct list_head *temp = &first_block.list;
        struct mem_block *block;
        while (temp != NULL) {
                block = (struct mem_block*)((void*)temp - offsetof(struct mem_block, list));
                if (block->sz == sz && block->free == 1)
                        return block;
                temp = temp->next;
        }
        return NULL;
}

static struct mem_block* _find_block(void *ptr) {
        struct list_head *temp = &first_block.list;
        struct mem_block *block;
        while (temp != NULL) {
                block = (struct mem_block*)((void*)temp - offsetof(struct mem_block, list));
                if (block->ptr == ptr)
                        return block;
                temp = temp->next;
        }
        return NULL;
}

static struct mem_block* _alloc_block(size_t sz) {
        if (first_block.ptr == NULL) {
                first_block.ptr == DEADBLOCK;
                first_block.sz = 0;
        }

        struct mem_block *ret = _find_free_block(sz);
        if (ret != NULL) {
                ret->free = 0;
                return ret->ptr;
        }

        ret = malloc(sizeof(struct mem_block));
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

static void _free_block(struct mem_block *block, int hard) {
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

        struct mem_block *block = _find_free_block(sz);
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

        struct mem_block *block = _find_free_block(sz);
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

        struct mem_block *old = _find_block(ptr);
        struct mem_block *new = _alloc_block(sz);
        memcpy(new->ptr, old->ptr, old->sz);
        old->free = 1;
        return new->ptr;
}

void rune_free(void *ptr) {
        if (ptr == NULL)
                return;

        struct mem_block *block = _find_block(ptr);
        if (block->free == 1)
                return;
        block->free = 1;
}

void rune_free_all(void) {
        struct list_head *temp = &first_block.list;
        struct mem_block *block;
        while (temp != NULL) {
                block = (struct mem_block*)((void*)temp - offsetof(struct mem_block, list));
                if (block->ptr == DEADBLOCK) {
                        temp = temp->next;
                        continue;
                }

                temp = temp->next;
                if (block->ptr != NULL)
                        _free_block(block, 1);
        }
}
