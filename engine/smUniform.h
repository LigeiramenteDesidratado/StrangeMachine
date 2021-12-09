#ifndef SM_UNIFORM_H
#define SM_UNIFORM_H

#include "util/common.h"

// Forward declaration
void __uniform_default_value(uint32_t slot, void *data);
void __uniform_default_array(uint32_t slot, void *data, uint32_t length);

// values
void __uniform_set_value_int(uint32_t slot, int data);
void __uniform_set_value_ivec4(uint32_t slot, ivec4 data);
void __uniform_set_value_ivec2(uint32_t slot, ivec2 data);
void __uniform_set_value_float(uint32_t slot, float data);
void __uniform_set_value_vec2(uint32_t slot, vec2 data);
void __uniform_set_value_vec3(uint32_t slot, vec3 data);
void __uniform_set_value_vec4(uint32_t slot, vec4 data);
void __uniform_set_value_quat(uint32_t slot, quat data);
void __uniform_set_value_mat4(uint32_t slot, mat4 data);

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
void __uniform_set_array_int(unsigned int slot, int *data, unsigned int length);
void __uniform_set_array_ivec4(unsigned int slot, ivec4 *data,
                               unsigned int length);
void __uniform_set_array_ivec2(unsigned int slot, ivec2 *data,
                               unsigned int length);
void __uniform_set_array_float(unsigned int slot, float *data,
                               unsigned int length);
void __uniform_set_array_vec2(unsigned int slot, vec2 *data,
                              unsigned int length);
void __uniform_set_array_vec3(unsigned int slot, vec3 *data,
                              unsigned int length);
void __uniform_set_array_vec4(unsigned int slot, vec4 *data,
                              unsigned int length);
void __uniform_set_array_quat(unsigned int slot, quat *data,
                              unsigned int length);
void __uniform_set_array_mat4(unsigned int slot, mat4 *data,
                              unsigned int length);

#define uniform_set_array(SLOT, DATA, LENGTH)                                  \
  _Generic(&((DATA)[0]), int*                                                            \
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
