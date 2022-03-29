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

// bool attribute_ctor(attribute_s *attribute, type_kind_e kind) {
//
//   SM_ASSERT(attribute != NULL);
//
//   glGenBuffers(1, &attribute->vbo);
//   attribute->length = 0;
//   attribute->kind = kind;
//
//   return true;
// }
//
// struct Statistics stats = {.uploads = 0, .frames = 0};
//
// void attribute_dtor(attribute_s *attribute) {
//   SM_ASSERT(attribute != NULL);
//
//   glDeleteBuffers(1, &attribute->vbo);
// }
//
// #define DEFINE_ATTRIBUTE_SET(X) \
//   void attribute_set_##X(attribute_s *const attr, const X *const input_array, size_t array_length, GLenum usage) { \
//     SM_ASSERT(attr != NULL); \
//     attr->length = array_length; \
//     size_t size = sizeof(X); \
//     glBindBuffer(GL_ARRAY_BUFFER, attr->vbo); \
//     glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(size * attr->length), input_array, usage); \
//     glBindBuffer(GL_ARRAY_BUFFER, 0); \
//     stats.uploads++; \
//   }
//
// DEFINE_ATTRIBUTE_SET(int)
// DEFINE_ATTRIBUTE_SET(float)
// DEFINE_ATTRIBUTE_SET(vec2)
// DEFINE_ATTRIBUTE_SET(vec3)
// DEFINE_ATTRIBUTE_SET(vec4)
// DEFINE_ATTRIBUTE_SET(ivec4)
//
// void attribute_default(const attribute_s *const attr, const void *const array, uint32_t length, GLenum usage) {
//   (void)attr;
//   (void)array;
//   (void)length;
//   (void)usage;
//
//   SM_LOG_WARN("invalid attribute set");
// }
//
// void attribute_bind_to(attribute_s const *attribute, uint8_t slot) {
//
//   attribute_bind_to_pro(attribute, slot, 0, 0);
// }
//
// void attribute_bind_to_pro(attribute_s const *attribute, uint8_t slot, GLsizei stride, const void *pointer) {
//   SM_ASSERT(attribute != NULL);
//
//   glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);
//   glEnableVertexAttribArray(slot);
//
//   switch (attribute->kind) {
//   case INT_KIND: /* Fallthrough */
//   case FLOAT_KIND:
//     glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, stride, pointer);
//     break;
//   case IVEC4_KIND: /* Fallthrough */
//   case VEC4_KIND:
//     glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, stride, pointer);
//     break;
//   case VEC2_KIND:
//     glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, stride, pointer);
//     break;
//   case VEC3_KIND:
//     glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, stride, pointer);
//     break;
//
//   default:
//     SM_LOG_WARN("unkown kind type");
//   }
//
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
// }
//
// void attribute_unbind_from(attribute_s const *attribute, uint8_t slot) {
//
//   SM_ASSERT(attribute != NULL);
//
//   glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);
//   glDisableVertexAttribArray(slot);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
// }
#undef SM_MODULE_NAME
