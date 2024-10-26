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

#ifndef RUNE_UTIL_LIST_H
#define RUNE_UTIL_LIST_H

#include <stddef.h>

/**
 * Linux-kernel combatible linked list implementation
 */
struct list_head {
        struct list_head *next; ///< Next list element
        struct list_head *prev; ///< Previous list element
};

/**
 * \brief Add element to the end of a list
 * \param[in] new Pointer to struct list_head, part of another struct
 * \param[in] head Start point of the list to be added to
 */
static inline void list_add(struct list_head *new, struct list_head *head) {
        struct list_head *temp = head;
        while (temp->next != NULL)
                temp = temp->next;

        temp->next = new;
        new->prev = temp;
        new->next = NULL;
}

/**
 * \brief Remove element from a list
 * \param[in] item Pointer to struct list_head, part of another struct
 */
static inline void list_del(struct list_head *item) {
        struct list_head *next = item->next;
        struct list_head *prev = item->prev;
        if (next != NULL)
                next->prev = prev;
        if (prev != NULL)
                prev->next = next;
        item->next = NULL;
        item->prev = NULL;
}

#endif
