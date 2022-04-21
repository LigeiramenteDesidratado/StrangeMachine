#ifndef SM_RENDERER_2D_H
#define SM_RENDERER_2D_H

#include "smpch.h"

#include "math/smMath.h"
#include "renderer/smDeviceDefs.h"
#include "resource/smTextureResourcePub.h"

struct renderer2D_s;

struct renderer2D_s *renderer2D_new(void);

bool renderer2D_ctor(struct renderer2D_s *renderer);
void renderer2D_dtor(struct renderer2D_s *renderer);
void renderer2D_begin(struct renderer2D_s *renderer);
void renderer2D_end(struct renderer2D_s *renderer);

void renderer2D_clear(struct renderer2D_s *renderer);
void renderer2D_set_clear_color(struct renderer2D_s *renderer, sm_vec4 color);

void renderer2D_draw_quad(struct renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color);
void renderer2D_draw_quad_rotated(struct renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color,
                                  float deg_angle);
void renderer2D_draw_sprite(struct renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler);
void renderer2D_draw_sprite_rotated(struct renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler,
                                    float deg_angle);

void renderer2D_set_viewport(struct renderer2D_s *renderer, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

uint32_t renderer2D_stats_get_quad_count(void);
uint32_t renderer2D_stats_get_draw_call_count(void);

#endif /* SM_RENDERER_2D_H */
