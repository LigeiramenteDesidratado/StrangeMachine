#ifndef SM_COLORS_H
#define SM_COLORS_H

#include "math/smMath.h"

/*
 * The following colors are taken from the X11 color database.
 * http://www.w3.org/TR/css3-color/#svg-color
 */

#define BLACK   sm_vec4_new(0.0f, 0.0f, 0.0f, 1.0f)
#define SILVER  sm_vec4_new(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f)
#define GRAY    sm_vec4_new(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f)
#define WHITE   sm_vec4_new(1.0f, 1.0f, 1.0f, 1.0f)
#define MAROON  sm_vec4_new(128.0f / 255.0f, 0.0f, 0.0f, 1.0f)
#define RED     sm_vec4_new(1.0f, 0.0f, 0.0f, 1.0f)
#define PURPLE  sm_vec4_new(128.0f / 255.0f, 0.0f, 128.0f / 255.0f, 1.0f)
#define FUCHSIA sm_vec4_new(1.0f, 0.0f, 1.0f, 1.0f)
#define GREEN   sm_vec4_new(0.0f, 128.0f / 255.0f, 0.0f, 1.0f)
#define LIME    sm_vec4_new(0.0f, 1.0f, 0.0f, 1.0f)
#define OLIVE   sm_vec4_new(128.0f / 255.0f, 128.0f / 255.0f, 0.0f, 1.0f)
#define YELLOW  sm_vec4_new(1.0f, 1.0f, 0.0f, 1.0f)
#define NAVY    sm_vec4_new(0.0f, 0.0f, 128.0f / 255.0f, 1.0f)
#define BLUE    sm_vec4_new(0.0f, 0.0f, 1.0f, 1.0f)
#define TEAL    sm_vec4_new(0.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f)
#define AQUA    sm_vec4_new(0.0f, 1.0f, 1.0f, 1.0f)

#define SM_BACKGROUND_COLOR sm_vec4_new(0x12 / (float)0xFF, 0x12 / (float)0xFF, 0x12 / (float)0xFF, 1.0f)
#define SM_FOREGROUND_COLOR sm_vec4_new(0xEE / (float)0xFF, 0xEE / (float)0xFF, 0xEE / (float)0xFF, 1.0f)
#define SM_MAIN_COLOR_0     sm_vec4_new(0x71 / (float)0xFF, 0xC8 / (float)0xFF, 0x71 / (float)0xFF, 1.0f)
#define SM_MAIN_COLOR_1     sm_vec4_new(0x85 / (float)0xFF, 0xDC / (float)0xFF, 0x85 / (float)0xFF, 1.0f)
#define SM_MAIN_COLOR_2     sm_vec4_new(0x99 / (float)0xFF, 0xF0 / (float)0xFF, 0x99 / (float)0xFF, 1.0f)
#define SM_RED_COLOR        sm_vec4_new(0x96 / (float)0xFF, 0x19 / (float)0xFF, 0x19 / (float)0xFF, 1.0f)

#endif /* SM_COLORS_H */
