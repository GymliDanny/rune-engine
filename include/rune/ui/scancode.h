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

#ifndef RUNE_UI_SCANCODE_H
#define RUNE_UI_SCANCODE_H

#define RUNE_SCANCODE_UNKNOWN   0x00

#define RUNE_SCANCODE_ESCAPE    0x01

#define RUNE_SCANCODE_F1        0x3B
#define RUNE_SCANCODE_F2        0x3C
#define RUNE_SCANCODE_F3        0x3D
#define RUNE_SCANCODE_F4        0x3E
#define RUNE_SCANCODE_F5        0x3F
#define RUNE_SCANCODE_F6        0x40
#define RUNE_SCANCODE_F7        0x41
#define RUNE_SCANCODE_F8        0x42
#define RUNE_SCANCODE_F9        0x43
#define RUNE_SCANCODE_F10       0x44
#define RUNE_SCANCODE_F11       0x57
#define RUNE_SCANCODE_F12       0x58

#define RUNE_SCANCODE_SYSRQ     0x63
#define RUNE_SCANCODE_SCLCK     0x46
#define RUNE_SCANCODE_PAUSE     0x77

#define RUNE_SCANCODE_TILDE     0x29
#define RUNE_SCANCODE_1         0x02
#define RUNE_SCANCODE_2         0x03
#define RUNE_SCANCODE_3         0x04
#define RUNE_SCANCODE_4         0x05
#define RUNE_SCANCODE_5         0x06
#define RUNE_SCANCODE_6         0x07
#define RUNE_SCANCODE_7         0x08
#define RUNE_SCANCODE_8         0x09
#define RUNE_SCANCODE_9         0x0A
#define RUNE_SCANCODE_0         0x0B
#define RUNE_SCANCODE_MINUS     0x0C
#define RUNE_SCANCODE_EQUAL     0x0D

#define RUNE_SCANCODE_Q         0x10
#define RUNE_SCANCODE_W         0x11
#define RUNE_SCANCODE_E         0x12
#define RUNE_SCANCODE_R         0x13
#define RUNE_SCANCODE_T         0x14
#define RUNE_SCANCODE_Y         0x15
#define RUNE_SCANCODE_U         0x16
#define RUNE_SCANCODE_I         0x17
#define RUNE_SCANCODE_O         0x18
#define RUNE_SCANCODE_P         0x19
#define RUNE_SCANCODE_A         0x1E
#define RUNE_SCANCODE_S         0x1F
#define RUNE_SCANCODE_D         0x20
#define RUNE_SCANCODE_F         0x21
#define RUNE_SCANCODE_G         0x22
#define RUNE_SCANCODE_H         0x23
#define RUNE_SCANCODE_J         0x24
#define RUNE_SCANCODE_K         0x25
#define RUNE_SCANCODE_L         0x26
#define RUNE_SCANCODE_Z         0x2C
#define RUNE_SCANCODE_X         0x2D
#define RUNE_SCANCODE_C         0x2E
#define RUNE_SCANCODE_V         0x2F
#define RUNE_SCANCODE_B         0x30
#define RUNE_SCANCODE_N         0x31
#define RUNE_SCANCODE_M         0x32

#define RUNE_SCANCODE_BACKSPACE 0x0E
#define RUNE_SCANCODE_TAB       0x0F
#define RUNE_SCANCODE_BACKSLASH 0x2B
#define RUNE_SCANCODE_RETURN    0x1C

#define RUNE_SCANCODE_LCTRL     0x1D
#define RUNE_SCANCODE_LSHIFT    0x2A
#define RUNE_SCANCODE_LALT      0x38
#define RUNE_SCANCODE_LWIN      0x7D

#define RUNE_SCANCODE_SPACE     0x39

#define RUNE_SCANCODE_RCTRL     0x61
#define RUNE_SCANCODE_RSHIFT    0x36
#define RUNE_SCANCODE_RALT      0x64
#define RUNE_SCANCODE_RWIN      0x7E
#define RUNE_SCANCODE_MENU      0x7F

#define RUNE_SCANCODE_LBRKT     0x1A
#define RUNE_SCANCODE_RBRKT     0x1B
#define RUNE_SCANCODE_SEMICOL   0x27
#define RUNE_SCANCODE_QUOTE     0x28
#define RUNE_SCANCODE_COMMA     0x33
#define RUNE_SCANCODE_PERIOD    0x34
#define RUNE_SCANCODE_SLASH     0x35

#define RUNE_SCANCODE_INSERT    0x6E
#define RUNE_SCANCODE_HOME      0x66
#define RUNE_SCANCODE_PGUP      0x68
#define RUNE_SCANCODE_DELETE    0x6F
#define RUNE_SCANCODE_END       0x6B
#define RUNE_SCANCODE_PGDN      0x6D

#define RUNE_SCANCODE_UP        0x67
#define RUNE_SCANCODE_DOWN      0x6C
#define RUNE_SCANCODE_LEFT      0x69
#define RUNE_SCANCODE_RIGHT     0x6A

#define RUNE_SCANCODE_NMLCK     0x45
#define RUNE_SCANCODE_NMDIV     0x62
#define RUNE_SCANCODE_NMMUL     0x37
#define RUNE_SCANCODE_NMMNS     0x4A
#define RUNE_SCANCODE_NMPLS     0x4E
#define RUNE_SCANCODE_NMENT     0x60
#define RUNE_SCANCODE_NMDEL     0x53
#define RUNE_SCANCODE_NM1       0x4F
#define RUNE_SCANCODE_NM2       0x50
#define RUNE_SCANCODE_NM3       0x51
#define RUNE_SCANCODE_NM4       0x4B
#define RUNE_SCANCODE_NM5       0x4C
#define RUNE_SCANCODE_NM6       0x4D
#define RUNE_SCANCODE_NM7       0x47
#define RUNE_SCANCODE_NM8       0x48
#define RUNE_SCANCODE_NM9       0x49
#define RUNE_SCANCODE_NM0       0x52

#endif
