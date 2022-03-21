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
void renderer2D_draw_quad(struct renderer2D_s *renderer, vec2 position, vec2 size, vec4 color, float tex_id);

#endif /* SM_RENDERER_2D_H */
