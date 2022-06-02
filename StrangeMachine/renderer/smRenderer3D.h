#ifndef SM_RENDERER_3D_H
#define SM_RENDERER_3D_H

#include "smpch.h"

#include "math/smMath.h"

void renderer3D_init(void);
void renderer3D_teardown(void);

void renderer3D_end(void);
void renderer3D_begin(void);

void renderer3D_draw_cube_transform(sm_transform_s transform, sm_vec4 color);
void renderer3D_draw_sphere(sm_transform_s transform, sm_vec4 color, int slices, int stacks);

void renderer3D_clear(void);
void renderer3D_set_clear_color(sm_vec4 color);

#endif /* SM_RENDERER_3D_H */
