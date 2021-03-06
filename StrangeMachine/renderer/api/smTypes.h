#ifndef SM_TYPES_H
#define SM_TYPES_H

struct shader_s;
struct index_buffer_s;
struct vertex_buffer_s;
struct texture_s;

typedef enum {
  /* scalar types */
  SM_F32 = 0x01,
  SM_I32,

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
  SM_MAT4,

  SM_SAMPLER2D,

} types_e;

#define SM_TYPE_TO_STR(TYPE)                                                                                           \
  ((TYPE) == SM_F32         ? "f32"                                                                                    \
   : (TYPE) == SM_I32       ? "i32"                                                                                    \
   : (TYPE) == SM_IVEC2     ? "ivec2"                                                                                  \
   : (TYPE) == SM_IVEC3     ? "ivec3"                                                                                  \
   : (TYPE) == SM_IVEC4     ? "ivec4"                                                                                  \
   : (TYPE) == SM_VEC2      ? "vec2"                                                                                   \
   : (TYPE) == SM_VEC3      ? "vec3"                                                                                   \
   : (TYPE) == SM_VEC4      ? "vec4"                                                                                   \
   : (TYPE) == SM_MAT2      ? "mat2"                                                                                   \
   : (TYPE) == SM_MAT3      ? "mat3"                                                                                   \
   : (TYPE) == SM_MAT4      ? "mat4"                                                                                   \
   : (TYPE) == SM_SAMPLER2D ? "sampler2D"                                                                              \
                            : "unknown")

typedef enum {

  SM_DEPTH_TEST = 0x0B71,

} enable_flags_e;

typedef enum {

  SM_DEPTH_BUFFER_BIT = 0x00000100,   /* GL_DEPTH_BUFFER_BIT   */
  SM_STENCIL_BUFFER_BIT = 0x00000400, /* GL_STENCIL_BUFFER_BIT */
  SM_COLOR_BUFFER_BIT = 0x00004000,   /* GL_COLOR_BUFFER_BIT */

} buffer_bit_e;

#endif /* SM_TYPES_H */
