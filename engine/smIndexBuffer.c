
#include "smIndexBuffer.h"
#include "util/common.h"

// Constructor
bool index_buffer_ctor(index_buffer_s *index_buffer) {

  assert(index_buffer != NULL);

  glGenBuffers(1, &index_buffer->ebo);
  index_buffer->count = 0;

  return true;
}

// Destructor
void index_buffer_dtor(index_buffer_s *index_buffer) {

  assert(index_buffer != NULL);

  glDeleteBuffers(1, &index_buffer->ebo);
}

void index_buffer_set(index_buffer_s *index_buffer, uint32_t *input_array, size_t length) {

  assert(index_buffer != NULL);

  index_buffer->count = length;
  size_t size = sizeof(uint32_t);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * index_buffer->count, input_array,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
