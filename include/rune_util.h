#ifndef RUNE_UTIL_H
#define RUNE_UTIL_H

#include <stdint.h>

static inline uint32_t clamp(uint32_t value, float min, float max) {
        const uint32_t ret = value < min ? min : value;
        return ret > max ? max : ret;
}

#endif
