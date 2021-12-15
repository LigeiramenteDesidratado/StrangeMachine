#include "smUniform.h"

#include "util/common.h"

void __uniform_set_value_int(int32_t slot, int32_t data) {
  __uniform_set_array_int(slot, (int32_t *)&data, 1);
}

void __uniform_set_value_ivec4(int32_t slot, ivec4 data) {
  __uniform_set_array_ivec4(slot, (ivec4 *)&data, 1);
}

void __uniform_set_value_ivec2(int32_t slot, ivec2 data) {
  __uniform_set_array_ivec2(slot, (ivec2 *)&data, 1);
}

void __uniform_set_value_float(int32_t slot, float data) {
  __uniform_set_array_float(slot, (float *)&data, 1);
}

void __uniform_set_value_vec2(int32_t slot, vec2 data) {
  __uniform_set_array_vec2(slot, (vec2 *)&data, 1);
}

void __uniform_set_value_vec3(int32_t slot, vec3 data) {
  __uniform_set_array_vec3(slot, (vec3 *)&data, 1);
}

void __uniform_set_value_vec4(int32_t slot, vec4 data) {
  __uniform_set_array_vec4(slot, (vec4 *)&data, 1);
}

void __uniform_set_value_quat(int32_t slot, quat data) {
  __uniform_set_array_quat(slot, (quat *)&data, 1);
}

void __uniform_set_value_mat4(int32_t slot, mat4 data) {
  __uniform_set_array_mat4(slot, (mat4 *)&data, 1);
}

// arrays
void __uniform_set_array_int(int32_t slot, int32_t *data, int32_t length) {
  glUniform1iv(slot, length, (int32_t *)&data[0]);
}

void __uniform_set_array_ivec4(int32_t slot, ivec4 *data, int32_t length) {
  glUniform4iv(slot, length, (int32_t *)&data[0]);
}

void __uniform_set_array_ivec2(int32_t slot, ivec2 *data, int32_t length) {
  glUniform2iv(slot, length, (int32_t *)&data[0]);
}

void __uniform_set_array_float(int32_t slot, float *data, int32_t length) {
  glUniform1fv(slot, length, (float *)&data[0]);
}

void __uniform_set_array_vec2(int32_t slot, vec2 *data, int32_t length) {
  glUniform2fv(slot, length, (float *)&data[0]);
}

void __uniform_set_array_vec3(int32_t slot, vec3 *data, int32_t length) {
  glUniform3fv(slot, length, (float *)&data[0]);
}

void __uniform_set_array_vec4(int32_t slot, vec4 *data, int32_t length) {
  glUniform4fv(slot, length, (float *)&data[0]);
}

void __uniform_set_array_quat(int32_t slot, quat *data, int32_t length) {
  glUniform4fv(slot, length, (float *)&data[0]);
}

void __uniform_set_array_mat4(int32_t slot, mat4 *data, int32_t length) {
  glUniformMatrix4fv(slot, length, GL_FALSE, (float *)&data[0]);
}

void __uniform_default_value(int32_t slot, void *data) {
  (void)slot;
  (void)data;
  log_warn("invalid uniform value type");
}

void __uniform_default_array(int32_t slot, void *data, int32_t length) {
  (void)slot;
  (void)data;
  (void)length;
  log_warn("invalid uniform array type");
}
