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

#ifndef RUNE_CORE_MOD_H
#define RUNE_CORE_MOD_H

#include <rune/util/types.h>
#include <rune/util/list.h>

#ifdef _WIN32

#include <windows.h>

#define REGISTER_MOD(name, init_func, exit_func, update_func)                   \
        void register_##name() {                                                \
                rune_register_mod(#name, init_func, exit_func, update_func);    \
        }                                                                       \
        BOOL WINAPI DllMain(HINSTANCE hndl, DWORD fdw, LPVOID lpv) {            \
                if (fdw == DLL_PROCESS_ATTACH)                                  \
                        register_##name();                                      \
        }

#else

#define REGISTER_MOD(name, init_func, exit_func, update_func)                   \
        __attribute__((constructor)) void register_##name() {                   \
                rune_register_mod(#name, init_func, exit_func, update_func);    \
        }

#endif

/// Function pointer, used by the mod struct
typedef void (*mod_func)(void);

/**
 * Class-like definition for in-game mod
 */
struct mod {
        const char *name;       ///< Name of the mod
        mod_func init_func;     ///< Mod initialization function, called by rune_init_mods
        mod_func exit_func;     ///< Mod exit function, called by rune_close_mods
        mod_func update_func;   ///< Mod update function, called at every frame
        struct list_head list;  ///< Linked list of all mod structs, used internally
};

/**
 * \brief Load all the mods from the mod folder, mods must be either DLLs on Windows,
 * or shared objects on Linux.
 */
RAPI void rune_load_mods(void);

/**
 * \brief Iterate over the list of mods and call each mod's init_func
 */
RAPI void rune_init_mods(void);

/**
 * \brief Iterate over the list of mods, call each mod's exit_func and release memory
 */
RAPI void rune_close_mods(void);

/**
 * \brief Mod registration function, called by a mod by way of the REGISTER_MOD
 * macro
 * \param[in] name Name of the mod (can include version information)
 * \param[in] init_func Mod init function, called by rune_init_mods
 * \param[in] exit_func Mod exit function, called by rune_exit_mods
 * \param[in] update_func Mod update function, called during each frame
 */
RAPI void rune_register_mod(const char *name, mod_func init_func, mod_func exit_func, mod_func update_func);

#endif
