#ifndef SM_UNIFORM_H
#define SM_UNIFORM_H

#include "smpch.h"

#include "core/smLog.h"
#include "math/smMath.h"

/* Forward declaration */
void __uniform_default(const char *type);
void __uniform_array_default(const char *type);

/* uniform_set_value modifies the value of a uniform variable. The location of the uniform
 * variable to be modified is specified by SLOT, which should be a value returned by glGetUniformLocation.
 * To summarise, it wraps glUniform{1|2|3|4}{f|i|ui} GL functions */
#define uniform_set_value(SLOT, DATA)                                                                                  \
  _Generic((DATA), int                                                                                                 \
           : glUniform1iv(SLOT, 1, (i32 *)&DATA[0]), ivec4                                                             \
           : glUniform4iv(SLOT, 1, (i32 *)&DATA[0]), ivec2                                                             \
           : glUniform2iv(SLOT, 1, (i32 *)&DATA[0]), float                                                             \
           : glUniform1fv(SLOT, 1, (f32 *)&DATA[0]), vec2                                                              \
           : glUniform2fv(SLOT, 1, (f32 *)&DATA[0]), vec3                                                              \
           : glUniform3fv(SLOT, 1, (f32 *)&DATA[0]), vec4        /* versor as well */                                  \
           : glUniform4fv(SLOT, 1, (f32 *)&DATA[0]), float(*)[4] /* mat4 */                                            \
           : glUniformMatrix4fv(SLOT, 1, GL_FALSE, DATA[0]), default                                                   \
           : SM_LOG_WARN("'%s' variable has an invalid type", #DATA))

/* uniform_set_array modifies a uniform variable array. The location of the uniform variable to be modified
 * is specified by SLOT, which should be a value returned by glGetUniformLocation. To summarise, it wraps
 * glUniform{1|2|3|4}{f|i|ui} GL functions see: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1930.htm */
#define uniform_set_array(SLOT, DATA, LENGTH)                                                                          \
  _Generic(&((DATA)[0]), int*                                                                                          \
           : glUniform1iv(SLOT, LENGTH, (i32 *)&DATA[0]), ivec4*                                                   \
           : glUniform4iv(SLOT, LENGTH, (i32 *)&DATA[0]), ivec2*                                                   \
           : glUniform2iv(SLOT, LENGTH, (i32 *)&DATA[0]), float*                                                   \
           : glUniform1fv(SLOT, LENGTH, (f32 *)&DATA[0]), vec2*                                                      \
           : glUniform2fv(SLOT, LENGTH, (f32 *)&DATA[0]), vec3*                                                      \
           : glUniform3fv(SLOT, LENGTH, (f32 *)&DATA[0]), vec4*                                                      \
           : glUniform4fv(SLOT, LENGTH, (f32 *)&DATA[0]),  mat4*                                                      \
           : glUniformMatrix4fv(SLOT, LENGTH, GL_FALSE, (f32 *)&DATA[0]), default                                    \
           : SM_LOG_WARN("'%s' variable has an invalid type", #DATA))

#endif /* SM_UNIFORM_H */
