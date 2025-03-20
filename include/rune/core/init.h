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

#ifndef RUNE_CORE_INIT_H
#define RUNE_CORE_INIT_H

#include <rune/util/types.h>

/**
 * \brief Main point of initialization, must be called before any other engine
 * function
 * \param[in] argc The same argc defined in the program's main function
 * \param[in] argv The same argv defined in the program's main function
 * \return 0, or a negative number indicating the error
 */
RAPI int rune_init(int argc, char* argv[]);

/**
 * \brief Shuts down the engine and cleans up any remaining memory
 */
RAPI void rune_exit(void);

#endif
