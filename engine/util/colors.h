#ifndef SM_COLORS_H
#define SM_COLORS_H

#include "math/smMath.h"

/*
 * The following colors are taken from the X11 color database.
 * http://www.w3.org/TR/css3-color/#svg-color
 */

#define BLACK   vec4_new(0.0f, 0.0f, 0.0f, 1.0f)
#define SILVER  vec4_new(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f)
#define GRAY    vec4_new(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f)
#define WHITE   vec4_new(1.0f, 1.0f, 1.0f, 1.0f)
#define MAROON  vec4_new(128.0f / 255.0f, 0.0f, 0.0f, 1.0f)
#define RED     vec4_new(1.0f, 0.0f, 0.0f, 1.0f)
#define PURPLE  vec4_new(128.0f / 255.0f, 0.0f, 128.0f / 255.0f, 1.0f)
#define FUCHSIA vec4_new(1.0f, 0.0f, 1.0f, 1.0f)
#define GREEN   vec4_new(0.0f, 128.0f / 255.0f, 0.0f, 1.0f)
#define LIME    vec4_new(0.0f, 1.0f, 0.0f, 1.0f)
#define OLIVE   vec4_new(128.0f / 255.0f, 128.0f / 255.0f, 0.0f, 1.0f)
#define YELLOW  vec4_new(1.0f, 1.0f, 0.0f, 1.0f)
#define NAVY    vec4_new(0.0f, 0.0f, 128.0f / 255.0f, 1.0f)
#define BLUE    vec4_new(0.0f, 0.0f, 1.0f, 1.0f)
#define TEAL    vec4_new(0.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f)
#define AQUA    vec4_new(0.0f, 1.0f, 1.0f, 1.0f)

#endif /* SM_COLORS_H */
