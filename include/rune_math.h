#ifndef RUNE_MATH_H
#define RUNE_MATH_H

struct vec2 {
        float x;
        float y;
};

struct vec3 {
        float x;
        float y;
        float z;
};

struct vec4 {
        float x;
        float y;
        float z;
        float w;
};

struct mat3 {
        struct vec3 v[3];
};

struct mat4 {
        struct vec3 v[4];
};

struct quat {
        float x;
        float y;
        float z;
        float w;
};

#endif
