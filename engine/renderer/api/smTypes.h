#ifndef SM_TYPES_H
#define SM_TYPES_H

#include "renderer/api/smDescriptor.h"

struct shader_s;
struct index_buffer_s;
struct vertex_buffer_s;
struct texture_s;

typedef enum {
  /* scalar types */
  SM_FLOAT = 0x01,
  SM_INT,

  /* vector types */
  SM_IVEC2,
  SM_IVEC3,
  SM_IVEC4,

  SM_VEC2,
  SM_VEC3,
  SM_VEC4,

  /* matrix types */
  SM_MAT2,
  SM_MAT3,
  SM_MAT4

} types_e;

#define SM_TYPE_TO_STR(TYPE)                                                                                           \
  ((TYPE) == SM_FLOAT   ? "float"                                                                                      \
   : (TYPE) == SM_INT   ? "int"                                                                                        \
   : (TYPE) == SM_IVEC2 ? "ivec2"                                                                                      \
   : (TYPE) == SM_IVEC3 ? "ivec3"                                                                                      \
   : (TYPE) == SM_IVEC4 ? "ivec4"                                                                                      \
   : (TYPE) == SM_VEC2  ? "vec2"                                                                                       \
   : (TYPE) == SM_VEC3  ? "vec3"                                                                                       \
   : (TYPE) == SM_VEC4  ? "vec4"                                                                                       \
   : (TYPE) == SM_MAT2  ? "mat2"                                                                                       \
   : (TYPE) == SM_MAT3  ? "mat3"                                                                                       \
   : (TYPE) == SM_MAT4  ? "mat4"                                                                                       \
                        : "unknown")

#endif /* SM_TYPES_H */
