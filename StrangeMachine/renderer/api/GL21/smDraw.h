#ifndef SM_DRAW_H
#define SM_DRAW_H

#include "smpch.h"

#include "renderer/api/smTypes.h"

void GL21enable(enable_flags_e flags);
void GL21clear_color(f32 r, f32 g, f32 b, f32 a);
void GL21clear(buffer_bit_e mask);
void GL21_viewport(u32 x, u32 y, u32 width, u32 height);
void GL21draw_indexed(u32 index_count);

#endif
