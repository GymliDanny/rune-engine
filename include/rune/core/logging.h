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

#ifndef RUNE_CORE_LOGGING_H
#define RUNE_CORE_LOGGING_H

#include <rune/util/types.h>

/// Indicates the error level of the message
enum log_level {
        LOG_FATAL,      ///< A major error has occured and execution cannot continue
        LOG_ERROR,      ///< A major error has occured
        LOG_WARN,       ///< A minor error has occured
        LOG_INFO,       ///< A simple event has occured
        LOG_DEBUG       ///< A confirmation or other debugging message, only printed when enabled
};

/**
 * \brief Print message to the engine log
 * \param[in] level Error level, a value in log_level
 * \param[in] fmt Format string
 * \param[in] ... Format specifiers (%d, %s, etc.)
 */
RAPI void log_output(int level, const char *fmt, ...);

/**
 * \brief Enable debug logging
 */
RAPI void enable_log_debug(void);

/**
 * \brief Disable debug logging (default)
 */
RAPI void disable_log_debug(void);

/**
 * \brief Enable color output (default)
 */
RAPI void enable_log_color(void);

/**
 * \brief Disable color output
 */
RAPI void disable_log_color(void);

#endif
