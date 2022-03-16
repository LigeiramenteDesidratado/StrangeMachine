#include "smAttribute.h"
#include "util/common.h"

bool attribute_ctor(attribute_s *attribute, type_kind_e kind) {

  SM_ASSERT(attribute != NULL);

  glGenBuffers(1, &attribute->vbo);
  attribute->length = 0;
  attribute->kind = kind;

  return true;
}

struct Statistics stats = {.uploads = 0, .frames = 0};

void attribute_dtor(attribute_s *attribute) {
  SM_ASSERT(attribute != NULL);

  glDeleteBuffers(1, &attribute->vbo);
}

#define DEFINE_ATTRIBUTE_SET(X)                                                                                        \
  void attribute_set_##X(attribute_s *const attr, const X *const input_array, size_t array_length, GLenum usage) {     \
    SM_ASSERT(attr != NULL);                                                                                           \
    attr->length = array_length;                                                                                       \
    size_t size = sizeof(X);                                                                                           \
    glBindBuffer(GL_ARRAY_BUFFER, attr->vbo);                                                                          \
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(size * attr->length), input_array, usage);                              \
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                                                                  \
    stats.uploads++;                                                                                                   \
  }

DEFINE_ATTRIBUTE_SET(int)
DEFINE_ATTRIBUTE_SET(float)
DEFINE_ATTRIBUTE_SET(vec2)
DEFINE_ATTRIBUTE_SET(vec3)
DEFINE_ATTRIBUTE_SET(vec4)
DEFINE_ATTRIBUTE_SET(ivec4)

void attribute_default(const attribute_s *const attr, const void *const array, uint32_t length, GLenum usage) {
  (void)attr;
  (void)array;
  (void)length;
  (void)usage;

  SM_LOG_WARN("invalid attribute set");
}

void attribute_bind_to(attribute_s const *attribute, uint8_t slot) {

  attribute_bind_to_pro(attribute, slot, 0, 0);
}

void attribute_bind_to_pro(attribute_s const *attribute, uint8_t slot, GLsizei stride, const void *pointer) {
  SM_ASSERT(attribute != NULL);

  glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);
  glEnableVertexAttribArray(slot);

  switch (attribute->kind) {
  case INT_KIND: /* Fallthrough */
  case FLOAT_KIND:
    glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, stride, pointer);
    break;
  case IVEC4_KIND: /* Fallthrough */
  case VEC4_KIND:
    glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, stride, pointer);
    break;
  case VEC2_KIND:
    glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, stride, pointer);
    break;
  case VEC3_KIND:
    glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, stride, pointer);
    break;

  default:
    SM_LOG_WARN("unkown kind type");
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void attribute_unbind_from(attribute_s const *attribute, uint8_t slot) {

  SM_ASSERT(attribute != NULL);

  glBindBuffer(GL_ARRAY_BUFFER, attribute->vbo);
  glDisableVertexAttribArray(slot);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
