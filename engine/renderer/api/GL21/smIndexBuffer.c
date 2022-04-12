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
  bool is_dynamic;    /* Is the buffer dynamic? */

} index_buffer_s;

index_buffer_s *GL21index_buffer_new(void) {
  index_buffer_s *index_buffer = SM_CALLOC(1, sizeof(index_buffer_s));

  SM_ASSERT(index_buffer);

  return index_buffer;
}

bool GL21index_buffer_ctor(index_buffer_s *index_buffer, buffer_desc_s *desc) {

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

void GL21index_buffer_set_data(index_buffer_s *index_buffer, uint32_t *data, size_t length) {

  SM_ASSERT(index_buffer);

  glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->EBO));

  if (index_buffer->is_dynamic) {
    glCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, length, data));

  } else {
    size_t size = length * sizeof(uint32_t);
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

// Constructor
// bool index_buffer_ctor(index_buffer_s *index_buffer) {
//
//   SM_ASSERT(index_buffer != NULL);
//
//   glGenBuffers(1, &index_buffer->ebo);
//   index_buffer->count = 0;
//
//   return true;
// }
//
// // Destructor
// void index_buffer_dtor(index_buffer_s *index_buffer) {
//
//   SM_ASSERT(index_buffer != NULL);
//
//   glDeleteBuffers(1, &index_buffer->ebo);
// }
//
// void index_buffer_set(index_buffer_s *index_buffer, uint32_t *input_array, size_t length) {
//
//   SM_ASSERT(index_buffer != NULL);
//
//   index_buffer->count = length;
//   size_t size = sizeof(uint32_t);
//
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->ebo);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * index_buffer->count, input_array, GL_STATIC_DRAW);
//
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }
