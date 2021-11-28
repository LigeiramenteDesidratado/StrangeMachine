#ifndef TEXTURE_H
#define TEXTURE_H

#include "util/common.h"

typedef struct {
  uint32_t width, height;
  uint32_t channels;
  GLuint texture;

} texture_s;

#define texture_new()                                                          \
  (texture_s) { .width = 0, .height = 0, .channels = 0, .texture = 0 }

// Constructor
status_v texture_ctor(texture_s *texture, const string path);

// Destructor
void texture_dtor(texture_s *texture);

// Forward declaration
void texture_set(texture_s const *texture, uint32_t uniform,
                 uint32_t tex_index);

void texture_unset(uint32_t tex_index);

#endif // TEXTURE_H
