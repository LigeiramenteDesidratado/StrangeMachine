#ifndef SM_UNIFORM_H
#define SM_UNIFORM_H

#include <stdint.h>

#include "math/mat4.h"
#include "math/quat.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

// Forward declaration
void __uniform_default(int32_t slot, void *data, int32_t length);

// uniform_set_value modifies the value of a uniform variable. The location of the uniform
// variable to be modified is specified by SLOT, which should be a value returned by glGetUniformLocation.
// To summarise, it wraps glUniform{1|2|3|4}{f|i|ui} GL functions
#define uniform_set_value(SLOT, DATA)                                                                                  \
  _Generic((DATA), int                                                                                                 \
           : glUniform1iv(SLOT, 1, (int32_t *)&DATA), ivec4                                                            \
           : glUniform4iv(SLOT, 1, (int32_t *)&DATA), ivec2                                                            \
           : glUniform2iv(SLOT, 1, (int32_t *)&DATA), float                                                            \
           : glUniform1fv(SLOT, 1, (float *)&DATA), vec2                                                               \
           : glUniform2fv(SLOT, 1, (float *)&DATA), vec3                                                               \
           : glUniform3fv(SLOT, 1, (float *)&DATA), vec4                                                               \
           : glUniform4fv(SLOT, 1, (float *)&DATA), quat                                                               \
           : glUniform4fv(SLOT, 1, (float *)&DATA), mat4                                                               \
           : glUniformMatrix4fv(SLOT, 1, GL_FALSE, (float *)&DATA), default                                            \
           : __uniform_default(0, (void *)&DATA, 0))

// uniform_set_array modifies a uniform variable array. The location of the uniform variable to be modified is specified
// by SLOT, which should be a value returned by glGetUniformLocation.
// To summarise, it wraps glUniform{1|2|3|4}{f|i|ui} GL functions
// see: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1930.htm
#define uniform_set_array(SLOT, DATA, LENGTH)                                                                          \
  _Generic(&((DATA)[0]), int*                                                                                          \
           : glUniform1iv(SLOT, LENGTH, (int32_t *)&DATA[0]), ivec4*                                                   \
           : glUniform4iv(SLOT, LENGTH, (int32_t *)&DATA[0]), ivec2*                                                   \
           : glUniform2iv(SLOT, LENGTH, (int32_t *)&DATA[0]), float*                                                   \
           : glUniform1fv(SLOT, LENGTH, (float *)&DATA[0]), vec2*                                                      \
           : glUniform2fv(SLOT, LENGTH, (float *)&DATA[0]), vec3*                                                      \
           : glUniform3fv(SLOT, LENGTH, (float *)&DATA[0]), vec4*                                                      \
           : glUniform4fv(SLOT, LENGTH, (float *)&DATA[0]), quat*                                                      \
           : glUniform4fv(SLOT, LENGTH, (float *)&DATA[0]), mat4*                                                      \
           : glUniformMatrix4fv(SLOT, LENGTH, GL_FALSE, (float *)&DATA[0]), default                                    \
           : __uniform_default(0, (void *)&DATA, 0))

#endif // SM_UNIFORM_H
