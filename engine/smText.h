#ifndef SM_TEXT_H
#define SM_TEXT_H

#include <stdint.h>

#include "math/vec2.h"
#include "math/vec3.h"

struct text_attr {
  const uint8_t position;
  const uint8_t tex_coord;
  const uint8_t color;
};
extern const struct text_attr text_attr_locs;


void text_init(void);
void text_draw(vec2 dest, int w, vec3 color, char *format, ...);
void text_flush(void);
void text_tear_down(void);

#endif // SM_TEXT_H
