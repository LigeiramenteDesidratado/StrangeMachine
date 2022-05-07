#ifndef SM_RENDERER_3D_H
#define SM_RENDERER_3D_H

#include "math/smMath.h"
#include "smpch.h"

struct sm_renderer3D_s;

struct sm_renderer3D_s *renderer3D_new(void);

bool renderer3D_ctor(struct sm_renderer3D_s *renderer);
void renderer3D_dtor(struct sm_renderer3D_s *renderer);
/* void renderer3D_draw_cube(struct sm_renderer3D_s *renderer); */

void renderer3D_end(struct sm_renderer3D_s *renderer);
void renderer3D_begin(struct sm_renderer3D_s *renderer);
/* void renderer3D_draw_cube_transform(struct sm_renderer3D_s *renderer, sm_transform_s transform); */
void renderer3D_draw_cube_transform(struct sm_renderer3D_s *renderer, sm_transform_s transform, sm_vec4 color);
void renderer3D_draw_sphere(struct sm_renderer3D_s *renderer, sm_transform_s transform, sm_vec4 color, int slices, int stacks);

void renderer3D_clear(struct sm_renderer3D_s *renderer);
void renderer3D_set_clear_color(struct sm_renderer3D_s *renderer, sm_vec4 color);

#endif /* SM_RENDERER_3D_H */
