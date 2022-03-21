#ifndef SM_RENDERER_2D_H
#define SM_RENDERER_2D_H

#include "smpch.h"

#include "math/smMath.h"
#include "renderer/smDeviceDefs.h"

struct renderer2D_s;

struct renderer2D_s *renderer2D_new(void);

bool renderer2D_ctor(struct renderer2D_s *renderer, device_api_e device);
void renderer2D_dtor(struct renderer2D_s *renderer);
void renderer2D_begin(struct renderer2D_s *renderer);
void renderer2D_end(struct renderer2D_s *renderer);

void renderer2D_clear(struct renderer2D_s *renderer);
void renderer2D_set_clear_color(struct renderer2D_s *renderer, vec4 color);

void renderer2D_draw_quad(struct renderer2D_s *renderer, vec2 position, vec2 size, vec4 color, float tex_id);
void renderer2D_draw_quad_rotated(struct renderer2D_s *renderer, vec2 position, vec2 size, vec4 color, float tex_id,
                                  float deg_angle);

#endif /* SM_RENDERER_2D_H */
