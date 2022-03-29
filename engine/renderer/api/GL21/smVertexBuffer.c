#include "smpch.h"

#include "core/smCore.h"

#include "renderer/api/GL21/smGLUtil.h"
#include "renderer/api/smDescriptor.h"

#include "vendor/gladGL21/glad.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

#define SM_MAX_ATTRIBUTES 8

typedef struct {

  GLuint VBO; /* Vertex Buffer Object */

  uint8_t attr_count; /* Number of attributes */
  uint8_t attrs[SM_MAX_ATTRIBUTES];

  size_t buffer_size; /* Size of the buffer */
  bool is_dynamic;    /* Is the buffer dynamic? */

} vertex_buffer_s;

vertex_buffer_s *GL21vertex_buffer_new(void) {

  vertex_buffer_s *vertex_buffer = SM_CALLOC(1, sizeof(vertex_buffer_s));

  return vertex_buffer;
}

bool GL21vertex_buffer_ctor(vertex_buffer_s *vertex_buffer, buffer_desc_s *desc) {

  SM_ASSERT(vertex_buffer != NULL);
  SM_ASSERT(desc != NULL);
  SM_ASSERT(vertex_buffer->VBO == 0 && "Vertex buffer already initialized");

  glCall(glGenBuffers(1, &vertex_buffer->VBO));
  glCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->VBO));
  glCall(
      glBufferData(GL_ARRAY_BUFFER, desc->buffer_size, desc->data, (desc->dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
  glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

  vertex_buffer->buffer_size = desc->buffer_size;
  vertex_buffer->is_dynamic = desc->dynamic;

  return true;
}

void GL21vertex_buffer_dtor(vertex_buffer_s *vertex_buffer) {

  SM_ASSERT(vertex_buffer != NULL);

  glCall(glDeleteBuffers(1, &vertex_buffer->VBO));

  vertex_buffer->VBO = 0;
  vertex_buffer->attr_count = 0;
  vertex_buffer->buffer_size = 0;

  SM_FREE(vertex_buffer);
}

void GL21vertex_buffer_set_pointer(vertex_buffer_s *vertex_buffer, attribute_desc_s *attributes, size_t length) {

  SM_ASSERT(vertex_buffer != NULL);
  SM_ASSERT(attributes != NULL);
  SM_ASSERT(length > 0 && length <= SM_MAX_ATTRIBUTES);

  glCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->VBO));

  for (size_t i = 0; i < length; ++i) {

    glCall(glEnableVertexAttribArray(attributes[i].index));
    glCall(glVertexAttribPointer(attributes[i].index, attributes[i].size, attributes[i].type, GL_FALSE,
                                 attributes[i].stride, attributes[i].pointer));

    vertex_buffer->attr_count++;
    vertex_buffer->attrs[i] = attributes[i].index;
  }

  glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void GL21vertex_buffer_set_data(vertex_buffer_s *vertex_buffer, void *data, size_t length) {

  SM_ASSERT(vertex_buffer != NULL);
  SM_ASSERT(data != NULL);

  glCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->VBO));

  if (vertex_buffer->is_dynamic) {

    SM_ASSERT(length <= vertex_buffer->buffer_size);
    glCall(glBufferSubData(GL_ARRAY_BUFFER, 0, length, data));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

  } else {

    glCall(glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
  }

  glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void GL21vertex_buffer_bind(vertex_buffer_s *vertex_buffer) {

  SM_ASSERT(vertex_buffer != NULL);

  glCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->VBO));

  for (uint8_t i = 0; i < vertex_buffer->attr_count; ++i) {

    glCall(glEnableVertexAttribArray(vertex_buffer->attrs[i]));
  }
}

void GL21vertex_buffer_unbind(vertex_buffer_s *vertex_buffer) {

  SM_ASSERT(vertex_buffer != NULL);

  glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

  for (uint8_t i = vertex_buffer->attr_count; i > 0; --i) {

    glCall(glDisableVertexAttribArray(vertex_buffer->attrs[i - 1]));
  }
}

#undef SM_MODULE_NAME
