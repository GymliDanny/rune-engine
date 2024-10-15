#include <rune/core/mod.h>
#include <rune/core/logging.h>
#include <rune/core/alloc.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>

struct list_head *mods = NULL;

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
        struct list_head *temp = mods;
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

        struct list_head *temp = mods;
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
