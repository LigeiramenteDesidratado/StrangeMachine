#ifndef SM_DEBUG_H
#define SM_DEBUG_H

#include "util/common.h"
#include "smShapes.h"

status_v debug_init(void);

void debug_tear_down(void);

void debug_draw_aabb(bounding_box_s aabb, vec3 color);
void debug_draw_line(vec3 from, vec3 to, vec3 color);
void debug_draw_capsule(capsule_s c);


#endif // SM_DEBUG_H
