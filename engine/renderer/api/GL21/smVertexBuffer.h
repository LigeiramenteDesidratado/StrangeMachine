#ifndef SM_VERTEX_BUFFER_H
  #define SM_VERTEX_BUFFER_H

  #include "smpch.h"

  #include "renderer/api/smTypes.h"

struct vertex_buffer_s *GL21vertex_buffer_new(void);
bool GL21vertex_buffer_ctor(struct vertex_buffer_s *vertex_buffer, buffer_desc_s *desc);
void GL21vertex_buffer_dtor(struct vertex_buffer_s *vertex_buffer);
void GL21vertex_buffer_set_pointer(struct vertex_buffer_s *vertex_buffer, attribute_desc_s *attributes, size_t length);
void GL21vertex_buffer_set_data(struct vertex_buffer_s *vertex_buffer, void *data, size_t length);
void GL21vertex_buffer_bind(struct vertex_buffer_s *vertex_buffer);
void GL21vertex_buffer_unbind(struct vertex_buffer_s *vertex_buffer);

#endif /* SM_VERTEX_BUFFER_H */

// #ifndef SM_ATTRIBUTE_H
// #define SM_ATTRIBUTE_H
//
// #include <stdbool.h>
// #include <stddef.h>
//
// #include "glad/glad.h"
//
// #include "math/smMath.h"
//
// typedef enum {
//   FLOAT_KIND = 0x01,
//   INT_KIND = 0x02,
//   VEC2_KIND = 0x03,
//   VEC3_KIND = 0x04,
//   VEC4_KIND = 0x05,
//   IVEC4_KIND = 0x06
//
// } type_kind_e;
//
// typedef struct {
//   // vertex buffer objects
//   GLuint vbo;
//   size_t length;
//   type_kind_e kind;
//
// } attribute_s;
//
// struct Statistics {
//   uint32_t uploads;
//   uint32_t frames;
// };
//
// extern struct Statistics stats;
//
// #define attribute_new() ((attribute_s){.vbo = 0, .length = 0, .kind = 0})
//
// // Constructor
// bool attribute_ctor(attribute_s *attribute, type_kind_e kind);
//
// // Destructor
// void attribute_dtor(attribute_s *attribute);
//
// #define DECLARE_ATTRIBUTE_SET(X) \
//   void attribute_set_##X(attribute_s *const attr, const X *const input_array, size_t array_length, GLenum usage);
//
// DECLARE_ATTRIBUTE_SET(int)
// DECLARE_ATTRIBUTE_SET(float)
// DECLARE_ATTRIBUTE_SET(vec2)
// DECLARE_ATTRIBUTE_SET(vec3)
// DECLARE_ATTRIBUTE_SET(vec4)
// DECLARE_ATTRIBUTE_SET(ivec4)
// void attribute_default(const attribute_s *const attr, const void *const array, uint32_t length, GLenum usage);
//
// // see: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1930.htm
// #define attribute_set(ATTR, INPUT_ARRAY, ARRAY_LENGTH, USAGE) \
//   _Generic(&((INPUT_ARRAY)[0]), \
//       int*: attribute_set_int, \
//       float*: attribute_set_float, \
//       vec2*: attribute_set_vec2, \
//       vec3*: attribute_set_vec3, \
//       vec4*: attribute_set_vec4, \
//       ivec4*: attribute_set_ivec4, \ default : attribute_default)(ATTR, INPUT_ARRAY, ARRAY_LENGTH, USAGE)
//
// void attribute_bind_to_pro(attribute_s const *attribute, uint8_t slot, GLsizei stride, const void *pointer);
// void attribute_bind_to(attribute_s const *attribute, uint8_t slot);
// void attribute_unbind_from(attribute_s const *attribute, uint8_t slot);
//
// #endif // SM_ATTRIBUTE_H
