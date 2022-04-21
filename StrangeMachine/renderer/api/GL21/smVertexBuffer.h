#ifndef SM_VERTEX_BUFFER_H
#define SM_VERTEX_BUFFER_H

#include "smpch.h"

#include "renderer/api/smTypes.h"

struct vertex_buffer_s *GL21vertex_buffer_new(void);
bool GL21vertex_buffer_ctor(struct vertex_buffer_s *vertex_buffer, buffer_desc_s *desc);
void GL21vertex_buffer_dtor(struct vertex_buffer_s *vertex_buffer);
void GL21vertex_buffer_set_pointer(struct vertex_buffer_s *vertex_buffer, attribute_desc_s *attributes, size_t length);
void GL21vertex_buffer_set_data(struct vertex_buffer_s *vertex_buffer, const void *data, size_t length);
void GL21vertex_buffer_bind(struct vertex_buffer_s *vertex_buffer, uint32_t *locations, size_t length);
void GL21vertex_buffer_unbind(struct vertex_buffer_s *vertex_buffer, uint32_t *locations, size_t length);

#endif /* SM_VERTEX_BUFFER_H */
