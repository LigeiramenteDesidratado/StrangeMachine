#ifndef SM_DRAW_H
#define SM_DRAW_H

#include "renderer/api/smTypes.h"

void GL21enable(enable_flags_e flags);
void GL21clear_color(float r, float g, float b, float a);
void GL21clear(buffer_bit_e mask);
void GL21_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void GL21draw_indexed(uint32_t index_count);

#endif
