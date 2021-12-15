#ifndef SM_UNIFORM_H
#define SM_UNIFORM_H

#include <stdint.h>

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/quat.h"
#include "math/mat4.h"

// Forward declaration
void __uniform_default_value(int32_t slot, void *data);
void __uniform_default_array(int32_t slot, void *data, int32_t length);

// values
void __uniform_set_value_int(int32_t slot, int data);
void __uniform_set_value_ivec4(int32_t slot, ivec4 data);
void __uniform_set_value_ivec2(int32_t slot, ivec2 data);
void __uniform_set_value_float(int32_t slot, float data);
void __uniform_set_value_vec2(int32_t slot, vec2 data);
void __uniform_set_value_vec3(int32_t slot, vec3 data);
void __uniform_set_value_vec4(int32_t slot, vec4 data);
void __uniform_set_value_quat(int32_t slot, quat data);
void __uniform_set_value_mat4(int32_t slot, mat4 data);

#define uniform_set_value(SLOT, DATA)                                          \
  _Generic((DATA), int                                                         \
           : __uniform_set_value_int, ivec4                                    \
           : __uniform_set_value_ivec4, ivec2                                  \
           : __uniform_set_value_ivec2, float                                  \
           : __uniform_set_value_float, vec2                                   \
           : __uniform_set_value_vec2, vec3                                    \
           : __uniform_set_value_vec3, vec4                                    \
           : __uniform_set_value_vec4, quat                                    \
           : __uniform_set_value_quat, mat4                                    \
           : __uniform_set_value_mat4, default                                 \
           : __uniform_default_value)(SLOT, DATA)

// arrays
void __uniform_set_array_int(int32_t slot, int *data, int32_t length);
void __uniform_set_array_ivec4(int32_t slot, ivec4 *data, int32_t length);
void __uniform_set_array_ivec2(int32_t slot, ivec2 *data, int32_t length);
void __uniform_set_array_float(int32_t slot, float *data, int32_t length);
void __uniform_set_array_vec2(int32_t slot, vec2 *data, int32_t length);
void __uniform_set_array_vec3(int32_t slot, vec3 *data, int32_t length);
void __uniform_set_array_vec4(int32_t slot, vec4 *data, int32_t length);
void __uniform_set_array_quat(int32_t slot, quat *data, int32_t length);
void __uniform_set_array_mat4(int32_t slot, mat4 *data, int32_t length);

#define uniform_set_array(SLOT, DATA, LENGTH)                                  \
  _Generic(&((DATA)[0]), int*                                                   \
           : __uniform_set_array_int, ivec4*                                    \
           : __uniform_set_array_ivec4, ivec2*                                  \
           : __uniform_set_array_ivec2, float*                                  \
           : __uniform_set_array_float, vec2*                                   \
           : __uniform_set_array_vec2, vec3*                                    \
           : __uniform_set_array_vec3, vec4*                                    \
           : __uniform_set_array_vec4, quat*                                    \
           : __uniform_set_array_quat, mat4*                                    \
           : __uniform_set_array_mat4, default                                 \
           : __uniform_default_array)(SLOT, DATA, LENGTH)

#endif // SM_UNIFORM_H
