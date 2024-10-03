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

#ifndef RUNE_CORE_THREAD_H
#define RUNE_CORE_THREAD_H

#include <rune/util/types.h>
#include <rune/util/list.h>

struct thread {
        int ID;
        int detached;
        void *thread_handle;
        struct list_head list;
};

struct mutex {
        int ID;
        void *mutex_handle;
        struct list_head list;
};

RAPI void rune_init_thread_api(void);

RAPI int rune_thread_init(void* (*thread_fn)(void *data), void *data, int detached);
RAPI int rune_thread_cancel(int ID);
RAPI int rune_thread_join(int ID, void **retval);
RAPI int rune_thread_self(void);
RAPI void rune_thread_exit(void *retval);

RAPI int rune_mutex_init(void);
RAPI int rune_mutex_destroy(int ID);
RAPI int rune_mutex_lock(int ID);
RAPI int rune_mutex_unlock(int ID);

#endif
