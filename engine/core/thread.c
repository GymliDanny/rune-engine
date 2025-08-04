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

#include <rune/core/thread.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <pthread.h>
#include <string.h>
#include <stdatomic.h>

static list_head_t *threads = NULL;
static list_head_t *mutexes = NULL;
static int next_tid = 0;
static int next_mid = 0;

struct start_args {
        struct thread *thread;
        void* (*thread_fn)(void*);
        void *thread_args;
};

static struct thread* _find_thread_by_handle(void *handle) {
        if (threads == NULL)
                return NULL;

        list_head_t *temp = threads;
        struct thread *ret;
        while (temp != NULL) {
                ret = (struct thread*)((void*)temp - offsetof(struct thread, list));
                if (pthread_equal(*(pthread_t*)ret->thread_handle, *(pthread_t*)handle) == 1)
                        return ret;
                temp = temp->next;
        }
        return NULL;
}

static struct thread* _find_thread_by_id(int ID) {
        if (threads == NULL)
                return NULL;

        list_head_t *temp = threads;
        struct thread *ret;
        while (temp != NULL) {
                ret = (struct thread*)((void*)temp - offsetof(struct thread, list));
                if (ret->ID == ID)
                        return ret;
                temp = temp->next;
        }
        return NULL;
}

static struct mutex* _find_mutex_by_id(int ID) {
        if (mutexes == NULL)
                return NULL;

        list_head_t *temp = mutexes;
        struct mutex *ret;
        while (temp != NULL) {
                ret = (struct mutex*)((void*)temp - offsetof(struct mutex, list));
                if (ret->ID == ID)
                        return ret;
                temp = temp->next;
        }
        return NULL;
}


static void _cleanup_pthread(void *arg) {
        struct thread *thread = (struct thread*)arg;
        if (&thread->list != threads)
                list_del(&thread->list);
        rune_free(thread->thread_handle);
        rune_free(thread);
}

static void* _startup_pthread(void *arg) {
        struct start_args *start_args = (struct start_args*)arg;

        void* (*thread_fn)(void *data) = start_args->thread_fn;
        pthread_cleanup_push(_cleanup_pthread, start_args->thread);
        if (thread_fn != NULL)
                (*thread_fn)(start_args->thread_args);
        pthread_cleanup_pop(1);
        return NULL;
}

void rune_init_thread_api(void) {
        struct thread *start_thread = rune_alloc(sizeof(struct thread));
        start_thread->ID = next_tid++;
        start_thread->detached = 0;
        start_thread->thread_handle = rune_alloc(sizeof(pthread_t));
        *(pthread_t*)start_thread->thread_handle = pthread_self();
        pthread_cleanup_push(_cleanup_pthread, threads);
        pthread_cleanup_pop(0);
}

int rune_thread_init(void* (*thread_fn)(void *data), void *data, int detached) {
        struct thread *thread = rune_alloc(sizeof(struct thread));
        thread->ID = next_tid++;
        thread->detached = detached;
        thread->thread_handle = rune_alloc(sizeof(pthread_t));
        if (threads == NULL)
                threads = &thread->list;
        else
                list_add(&thread->list, threads);

        struct start_args *args = rune_alloc(sizeof(struct start_args));
        args->thread = thread;
        args->thread_fn = thread_fn;
        args->thread_args = data;
        int retval = pthread_create(thread->thread_handle, NULL, _startup_pthread, args);
        rune_free(args);
        if (retval != 0) {
                log_output(LOG_ERROR, "Thread creation failed: %s", strerror(retval));
                return -1;
        }
        log_output(LOG_DEBUG, "Initialized new thread with ID=%d", thread->ID);
        if (detached == 1) {
                log_output(LOG_DEBUG, "Thread %d has been detached, join no longer possible");
                pthread_detach(*(pthread_t*)thread->thread_handle);
        }
        return thread->ID;
}

int rune_thread_cancel(int ID) {
        log_output(LOG_DEBUG, "Received cancel request for thread %d", ID);
        struct thread *thread = _find_thread_by_id(ID);
        if (thread == NULL) {
                log_output(LOG_ERROR, "Thread %d does not exist", ID);
                return -1;
        }
        pthread_cancel(*((pthread_t*)thread->thread_handle));
        return 0;
}

int rune_thread_join(int ID, void **retval) {
        struct thread *thread = _find_thread_by_id(ID);
        if (thread == NULL)
                return 0;

        if (thread->detached == 1) {
                log_output(LOG_ERROR, "Cannot join thread %d, detached", thread->ID);
                return -1;
        }
        pthread_join(*((pthread_t*)thread->thread_handle), retval);
        return 0;
}

int rune_thread_self(void) {
        pthread_t cur = pthread_self();
        struct thread *ret = _find_thread_by_handle((void*)&cur);
        if (ret != NULL)
                return ret->ID;
        return -1;
}

void rune_thread_exit(void *retval) {
        pthread_t cur = pthread_self();
        struct thread *self = _find_thread_by_handle((void*)&cur);
        log_output(LOG_DEBUG, "Thread %d called thread_exit", self->ID);
        pthread_exit(retval);
}

int rune_mutex_init(void) {
        struct mutex *mutex = rune_alloc(sizeof(struct mutex));
        mutex->ID = next_mid++;
        mutex->mutex_handle = rune_alloc(sizeof(pthread_mutex_t));
        pthread_mutex_init((pthread_mutex_t*)mutex->mutex_handle, NULL);
        if (mutexes == NULL)
                mutexes = &mutex->list;
        else
                list_add(&mutex->list, mutexes);
        return mutex->ID;
}

int rune_mutex_destroy(int ID) {
        struct mutex *mutex = _find_mutex_by_id(ID);
        rune_free(mutex->mutex_handle);
        if (&mutex->list != mutexes)
                list_del(&mutex->list);
        rune_free(mutex);
}

int rune_mutex_lock(int ID) {
        struct mutex *mutex = _find_mutex_by_id(ID);
        int retval = pthread_mutex_lock((pthread_mutex_t*)mutex->mutex_handle);
        if (retval != 0) {
                char *str = strerror(retval);
                log_output(LOG_ERROR, "Cannot lock mutex %d: %s", mutex->ID, str);
        }
}

int rune_mutex_unlock(int ID) {
        struct mutex *mutex = _find_mutex_by_id(ID);
        int retval = pthread_mutex_unlock((pthread_mutex_t*)mutex->mutex_handle);
        if (retval != 0) {
                char *str = strerror(retval);
                log_output(LOG_ERROR, "Cannot unlock mutex %d: %s", mutex->ID, str);
        }
}
