#ifndef SM_INDEX_BUFFER_H
#define SM_INDEX_BUFFER_H

#include "smpch.h"

#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"

struct index_buffer_s *GL21index_buffer_new(void);
b8 GL21index_buffer_ctor(struct index_buffer_s *index_buffer, buffer_desc_s *desc);
void GL21index_buffer_dtor(struct index_buffer_s *index_buffer);
void GL21index_buffer_set_data(struct index_buffer_s *index_buffer, u32 *data, size_t length);
void GL21index_buffer_bind(struct index_buffer_s *index_buffer);
void GL21index_buffer_unbind(struct index_buffer_s *index_buffer);

#endif /* SM_INDEX_BUFFER_H */
