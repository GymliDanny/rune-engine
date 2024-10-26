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

/**
 * Platform-agnostic thread handle
 */
struct thread {
        int ID;                 ///< In-engine thread ID
        int detached;           ///< 1 if thread has been detached, 0 otherwise
        void *thread_handle;    ///< System-defined thread handle, usually a pthread_t
        struct list_head list;  ///< Linked list of all threads, used internally
};

/**
 * Platform-agnostic mutex handle
 */
struct mutex {
        int ID;                 ///< In-engine mutex ID
        void *mutex_handle;     ///< System-defined mutex handle, usually a pthread_mutex_t
        struct list_head list;  ///< Linked list of all mutexes, used internally
};

/**
 * \brief Initializes the engine's thread API, must be called before using any
 * API function
 */
RAPI void rune_init_thread_api(void);

/**
 * \brief Creates and starts a new thread
 * \param[in] thread_fn The function the thread will execute
 * \param[in] data Arguments for the thread_fn
 * \param[in] detached Whether the thread will start detached
 * \return ID of created thread, or -1 if error
 */
RAPI int rune_thread_init(void* (*thread_fn)(void *data), void *data, int detached);

/**
 * \brief Force-stops a thread
 * \param[in] ID Thread to stop
 * \return 0, or -1 if thread cannot be found
 */
RAPI int rune_thread_cancel(int ID);

/**
 * \brief Joins a thread to the calling thread
 * \param[in] ID Thread to wait on
 * \param[out] retval Return value of thread_fn
 */
RAPI int rune_thread_join(int ID, void **retval);

/**
 * \brief Gets the current thread ID
 * \return Current in-engine ID of the calling thread, or -1 on error
 */
RAPI int rune_thread_self(void);

/**
 * \brief Terminates the calling thread
 * \param[in] retval Where to put the thread_fn return value
 */
RAPI void rune_thread_exit(void *retval);

/**
 * \brief Creates a new mutex
 * \return ID of new mutex, or -1 on error
 */
RAPI int rune_mutex_init(void);

/**
 * \brief Cleans up a mutex and releases its memory
 * \param[in] ID Mutex to destroy
 */
RAPI int rune_mutex_destroy(int ID);

/**
 * \brief Locks a mutex
 * \param[in] ID Mutex to lock
 */
RAPI int rune_mutex_lock(int ID);

/**
 * \brief Unlocks a mutex
 * \param[in] ID Mutex to unlock
 */
RAPI int rune_mutex_unlock(int ID);

#endif
