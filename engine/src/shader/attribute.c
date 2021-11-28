#include "shader/attribute.h"
#include "util/common.h"

status_v attribute_ctor(attribute_s *attribute, EX1 kind) {

  assert(attribute != NULL);

  glGenBuffers(1, &attribute->vbo);
  attribute->length = 0;
  attribute->kind = kind;

  return ok;
}

void attribute_dtor(attribute_s *attribute) {
  assert(attribute != NULL);

  glDeleteBuffers(1, &attribute->vbo);
}

#define DEFINE_ATTRIBUTE_SET(X)                                                \
  void attribute_set_##X(attribute_s *const attribute,                         \
                         const X *const input_array, uint32_t array_length,    \
                         GLenum usage) {                                       \
    assert(attribute != NULL);                                                 \
    attribute->length = array_length;                                          \
    size_t size = sizeof(X);                                                   \
    glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);                             \
    glBufferData(GL_ARRAY_BUFFER, size * attribute->length, input_array,       \
                 usage);                                                       \
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                          \
  }

DEFINE_ATTRIBUTE_SET(int)
DEFINE_ATTRIBUTE_SET(float)
DEFINE_ATTRIBUTE_SET(vec2)
DEFINE_ATTRIBUTE_SET(vec3)
DEFINE_ATTRIBUTE_SET(vec4)
DEFINE_ATTRIBUTE_SET(ivec4)

void attribute_default(const attribute_s *const attribute,
                       const void *const array, uint32_t length, GLenum usage) {
  (void)attribute;
  (void)array;
  (void)length;
  (void)usage;

  log_warn("invalid attribute set");
}

void attribute_bind_to(attribute_s const *attribute, uint8_t slot) {
  assert(attribute != NULL);

  glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);
  glEnableVertexAttribArray(slot);

  switch (attribute->kind) {
  case INT_EX1:
    glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, (void *)0);
    break;
  case IVEC4_EX1:
    glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    break;
  case FLOAT_EX1:
    glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, 0);
    break;
  case VEC2_EX1:
    glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, 0);
    break;
  case VEC3_EX1:
    glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, 0);
    break;
  case VEC4_EX1:
    glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, 0);
    break;

  default:
    log_warn("unkown kind type");
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void attribute_unbind_from(attribute_s const *attribute, uint8_t slot) {

  assert(attribute != NULL);

  glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);
  glDisableVertexAttribArray(slot);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
