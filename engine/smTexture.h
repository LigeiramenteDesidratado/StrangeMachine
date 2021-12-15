#ifndef SM_TEXTURE_H
#define SM_TEXTURE_H

#include <stdbool.h>
#include <stdint.h>

#include "glad/glad.h"

typedef struct {
  uint32_t width, height;
  uint32_t channels;
  GLuint texture;

} texture_s;

#define texture_new()                                                          \
  (texture_s) { .width = 0, .height = 0, .channels = 0, .texture = 0 }

// Constructor
bool texture_ctor(texture_s *texture, const char* path);

// Destructor
void texture_dtor(texture_s *texture);

// Forward declaration
void texture_set(texture_s const *texture, int32_t uniform, int32_t tex_index);

void texture_unset(uint32_t tex_index);

#endif // SM_TEXTURE_H
