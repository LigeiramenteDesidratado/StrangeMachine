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
void renderer2D_set_viewport(u32 x, u32 y, u32 width, u32 height);

void renderer2D_draw_quad(vec2 position, vec2 size, sm_vec4 color);
void renderer2D_draw_quad_rotated(vec2 position, vec2 size, sm_vec4 color, f32 deg_angle);
void renderer2D_draw_sprite(vec2 position, vec2 size, sm_texture_resource_handler_s handler);
void renderer2D_draw_sprite_rotated(vec2 position, vec2 size, sm_texture_resource_handler_s handler, f32 deg_angle);

u32 renderer2D_stats_get_quad_count(void);
u32 renderer2D_stats_get_draw_call_count(void);

#endif /* SM_RENDERER_2D_H */
