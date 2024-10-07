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

void* rune_alloc(size_t sz) {
        if (sz == 0)
                return NULL;

        if (first_block.ptr == NULL) {
                first_block.ptr = DEADBLOCK;
                first_block.sz = 0;
        }

        struct mem_block *block = _find_free_block(sz);
        if (block != NULL) {
                block->free = 0;
                return block->ptr;
        }

        block = malloc(sizeof(struct mem_block));
        if (block == NULL) {
                log_output(LOG_ERROR, "Cannot allocate block of size %d", sz);
                return NULL;
        }
        block->ptr = malloc(sz);
        block->sz = sz;
        block->free = 0;
        list_add(&block->list, &first_block.list);
        log_output(LOG_DEBUG, "Alloc'd block of size %d", sz);
        return block->ptr;
}

void* rune_calloc(size_t nmemb, size_t sz) {
        void *ret = rune_alloc(sz);
        for (size_t *i = (size_t*)ret; i < (size_t*)ret+sz; i++)
                *i = nmemb;
        return ret;
}

void* rune_realloc(void *ptr, size_t sz) {
        if (ptr == NULL || sz == 0)
                return rune_alloc(sz);

        struct mem_block *block = _find_block(ptr);
        void *new_ptr = rune_alloc(sz);
        memcpy(new_ptr, ptr, block->sz);
        block->free = 1;
        return new_ptr;
}

void rune_free(void *ptr) {
        if (ptr == NULL)
                return;

        struct mem_block *block = _find_block(ptr);
        if (block->free == 1)
                return;
        block->free = 1;
        log_output(LOG_DEBUG, "Freed block of size %d", block->sz);
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

                if (block->ptr != NULL)
                        free(block->ptr);
                temp = temp->next;
        }
}
