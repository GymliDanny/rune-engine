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

#ifndef RUNE_UI_INPUT_H
#define RUNE_UI_INPUT_H

#include <rune/util/types.h>
#include <rune/ui/window.h>

#define KB_MODE_RAW     0
#define KB_MODE_TEXT    1

RAPI int rune_input_init(struct rune_window *window);

RAPI void set_keyboard_mode(int mode);
RAPI int get_keyboard_mode(void);

RAPI void register_key_hook(uint8_t scancode, void (*func)(void));
RAPI void rune_input_tick(void);

#endif
