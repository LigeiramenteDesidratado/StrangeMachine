#ifndef SM_RENDERER_3D_H
#define SM_RENDERER_3D_H

#include "smpch.h"

#include "math/smMath.h"
#include "scene/smComponents.h"

void sm_renderer3D_init(void);
void sm_renderer3D_teardown(void);

void sm_renderer3D_end(void);
/* void sm_renderer3D_begin(void); */
void sm_renderer3D_begin(sm_mat4 pv_matrix);

void sm_renderer3D_draw_cube_transform(sm_transform_s transform, sm_vec4 color);
void sm_renderer3D_draw_mesh(const sm_mesh_s *mesh, sm_transform_s transform);
/* void renderer3D_draw_mesh(mesh_s *mesh, sm_transform_s transform); */

void sm_renderer3D_clear(void);
void sm_renderer3D_set_clear_color(sm_vec4 color);

#endif /* SM_RENDERER_3D_H */
