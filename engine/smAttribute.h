#ifndef SM_ATTRIBUTE_H
#define SM_ATTRIBUTE_H

#include <stdbool.h>
#include <stddef.h>

#include "glad/glad.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

// ENUM X ID
typedef unsigned char EX1;
#define FLOAT_EX1 ((EX1)0x01)
#define INT_EX1 ((EX1)0x02)
#define VEC2_EX1 ((EX1)0x03)
#define VEC3_EX1 ((EX1)0x04)
#define VEC4_EX1 ((EX1)0x05)
#define IVEC4_EX1 ((EX1)0x06)

typedef struct {
  // vertex buffer objects
  GLuint vbo;
  size_t length;
  EX1 kind;

} attribute_s;

#define attribute_new() ((attribute_s){.vbo = 0, .length = 0, .kind = 0})

// Constructor
bool attribute_ctor(attribute_s *attribute, EX1 kind);

// Destructor
void attribute_dtor(attribute_s *attribute);

#define DECLARE_ATTRIBUTE_SET(X)                                               \
  void attribute_set_##X(attribute_s *const attribute,                         \
                         const X *const input_array, size_t array_length,    \
                         GLenum usage);

DECLARE_ATTRIBUTE_SET(int)
DECLARE_ATTRIBUTE_SET(float)
DECLARE_ATTRIBUTE_SET(vec2)
DECLARE_ATTRIBUTE_SET(vec3)
DECLARE_ATTRIBUTE_SET(vec4)
DECLARE_ATTRIBUTE_SET(ivec4)
void attribute_default(const attribute_s *const attribute,
                       const void *const array, uint32_t length, GLenum usage);

// see: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1930.htm
#define attribute_set(ATTR, INPUT_ARRAY, ARRAY_LENGTH, USAGE)                  \
  _Generic(&((INPUT_ARRAY)[0]), \
      int*: attribute_set_int, \
      float*: attribute_set_float, \
      vec2*: attribute_set_vec2, \
      vec3*: attribute_set_vec3, \
      vec4*: attribute_set_vec4, \
      ivec4*: attribute_set_ivec4, \
      default : attribute_default)(ATTR, INPUT_ARRAY, ARRAY_LENGTH, USAGE)

void attribute_bind_to(attribute_s const *attribute, uint8_t slot);
void attribute_unbind_from(attribute_s const *attribute, uint8_t slot);

#endif // SM_ATTRIBUTE_H
