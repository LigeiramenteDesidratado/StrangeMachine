#include "smpch.h"

#include "vendor/gladGL21/glad.h"

#include "renderer/api/GL21/smGLUtil.h"
#include "renderer/api/smDescriptor.h"

#include "core/smCore.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

typedef struct {
  GLuint EBO; /* Element Buffer Object */

  size_t buffer_size; /* Size of the buffer */
  b8 is_dynamic;      /* Is the buffer dynamic? */

} index_buffer_s;

index_buffer_s *GL21index_buffer_new(void) {
  index_buffer_s *index_buffer = SM_CALLOC(1, sizeof(index_buffer_s));

  SM_ASSERT(index_buffer);

  return index_buffer;
}

b8 GL21index_buffer_ctor(index_buffer_s *index_buffer, buffer_desc_s *desc) {

  SM_ASSERT(index_buffer);

  glCall(glGenBuffers(1, &index_buffer->EBO));
  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->EBO));
  glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc->buffer_size, desc->data,
                      (desc->dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

  index_buffer->buffer_size = desc->buffer_size;
  index_buffer->is_dynamic = desc->dynamic;

  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

  return true;
}

void GL21index_buffer_dtor(index_buffer_s *index_buffer) {

  SM_ASSERT(index_buffer);

  glCall(glDeleteBuffers(1, &index_buffer->EBO));

  SM_FREE(index_buffer);
}

void GL21index_buffer_set_data(index_buffer_s *index_buffer, u32 *data, size_t length) {

  SM_ASSERT(index_buffer);

  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->EBO));

  if (index_buffer->is_dynamic) {
    glCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, length, data));

  } else {
    size_t size = length * sizeof(u32);
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    index_buffer->buffer_size = length;
  }

  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void GL21index_buffer_bind(index_buffer_s *index_buffer) {

  SM_ASSERT(index_buffer);

  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->EBO));
}

void GL21index_buffer_unbind(index_buffer_s *index_buffer) {

  SM_ASSERT(index_buffer);

  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
#undef SM_MODULE_NAME
