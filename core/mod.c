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

#include <rune/core/mod.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>

list_head_t *mods = NULL;

void _load_mod(const char *filename) {
        char mod_path[4096];
        sprintf(mod_path, "mods/%s", filename);
        void *handle = dlopen(mod_path, RTLD_LAZY);
        if (handle == NULL)
                log_output(LOG_ERROR, "Error loading mod %s: %s", filename, dlerror());
}

void rune_load_mods(void) {
        DIR *dir = opendir("mods");
        if (dir == NULL) {
                log_output(LOG_INFO, "No mods folder found, skipping mod loading");
                return;
        }

        struct dirent *mods_de;
        while ((mods_de = readdir(dir)) != NULL) {
                if (strstr(mods_de->d_name, ".so"))
                        _load_mod(mods_de->d_name);
        }
}

void rune_init_mods(void) {
        list_head_t *temp = mods;
        struct mod *mod;
        while (temp != NULL) {
                mod = (struct mod*)((void*)temp - offsetof(struct mod, list));
                (*mod->init_func)();
                temp = temp->next;
        }
}

void rune_close_mods(void) {
        if (mods == NULL)
                return;

        list_head_t *temp = mods;
        struct mod *mod;
        while (temp != NULL) {
                mod = (struct mod*)((void*)temp - offsetof(struct mod, list));
                (*mod->exit_func)();
                temp = temp->next;
                rune_free(mod);
        }
}

void rune_register_mod(const char *name, mod_func init_func, mod_func exit_func, mod_func update_func) {
        struct mod *new = rune_alloc(sizeof(struct mod));
        new->name = name;
        new->init_func = init_func;
        new->exit_func = exit_func;
        new->update_func = update_func;
        new->list.next = NULL;
        new->list.prev = NULL;
        if (mods == NULL)
                mods = &new->list;
        else
                list_add(&new->list, mods);
}
