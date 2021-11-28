#ifndef TEXT_H
#define TEXT_H

#include "util/common.h"

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

#endif // TEXT_H
