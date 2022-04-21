#ifndef SM_TEXTURE_H
#define SM_TEXTURE_H

#include "smpch.h"

struct texture_s;

struct texture_s *GL21texture_new(void);

// Constructor
bool GL21texture_ctor(struct texture_s *texture, uint32_t width, uint32_t height, void *data);

// Destructor
void GL21texture_dtor(struct texture_s *texture);
void GL21texture_dtor(struct texture_s *texture);

// Forward declaration
void GL21texture_bind(struct texture_s const *texture, uint32_t tex_index);
void GL21texture_unbind(struct texture_s const *texture, uint32_t tex_index);

#endif // SM_TEXTURE_H
