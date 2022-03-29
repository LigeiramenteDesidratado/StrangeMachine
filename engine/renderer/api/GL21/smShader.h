#ifndef SM_SHADER_H
#define SM_SHADER_H

#include "smpch.h"

#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"

struct shader_s *GL21shader_new(void);
bool GL21shader_ctor(struct shader_s *shader, const char *vertex_shader, const char *fragment_shader,
                     attribute_loc_desc_s *desc, size_t size);
void GL21shader_dtor(struct shader_s *shader);
void GL21shader_bind(struct shader_s *shader);
void GL21shader_unbind(struct shader_s *shader);
void GL21shader_set_uniform(struct shader_s *shader, const char *name, void *value, types_e type);
void GL21shader_set_uniform_array(struct shader_s *shader, const char *name, void *value, uint32_t size, types_e type);

#endif // SM_SHADER_H
