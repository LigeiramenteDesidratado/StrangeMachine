#ifndef SM_TEXTURE_H
#define SM_TEXTURE_H

#include "smpch.h"

#include "resource/smTextureResourcePub.h"

struct texture_s;

struct texture_s *GL21texture_new(void);

// Constructor
b8 GL21texture_ctor(struct texture_s *texture, sm_texture_resource_handler_s handler);

// Destructor
void GL21texture_dtor(struct texture_s *texture);

// Forward declaration
void GL21texture_bind(struct texture_s const *texture, u32 tex_index);
void GL21texture_unbind(struct texture_s const *texture, u32 tex_index);

#endif // SM_TEXTURE_H
