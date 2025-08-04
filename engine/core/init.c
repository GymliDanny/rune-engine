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

#include <rune/core/init.h>
#include <rune/core/abort.h>
#include <rune/core/alloc.h>
#include <rune/core/config.h>
#include <rune/core/logging.h>
#include <rune/core/thread.h>
#include <rune/core/mod.h>
#include <rune/core/object.h>

int rune_init(int argc, char* argv[]) {
        log_output(LOG_INFO, "Started Rune Engine version %s", RUNE_VER);

        rune_init_default_settings();
        rune_init_thread_api();

        rune_load_mods();
        rune_init_mods();

        return 0;
}

void rune_exit(void) {
        log_output(LOG_INFO, "Engine shutdown requested");
        rune_clear_objs();
        rune_close_mods();
        rune_free_all();
}
