#ifndef SM_RENDERER_2D_H
#define SM_RENDERER_2D_H

#include "smpch.h"

#include "math/smMath.h"
#include "resource/smTextureResourcePub.h"

void renderer2D_init(void);
void renderer2D_teardown(void);

void renderer2D_begin();
void renderer2D_end();

void renderer2D_clear();
void renderer2D_set_clear_color(sm_vec4 color);
void renderer2D_set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void renderer2D_draw_quad(vec2 position, vec2 size, sm_vec4 color);
void renderer2D_draw_quad_rotated(vec2 position, vec2 size, sm_vec4 color, float deg_angle);
void renderer2D_draw_sprite(vec2 position, vec2 size, texture_handler_s handler);
void renderer2D_draw_sprite_rotated(vec2 position, vec2 size, texture_handler_s handler, float deg_angle);

uint32_t renderer2D_stats_get_quad_count(void);
uint32_t renderer2D_stats_get_draw_call_count(void);

#endif /* SM_RENDERER_2D_H */
