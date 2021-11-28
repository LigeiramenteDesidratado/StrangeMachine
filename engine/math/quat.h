#ifndef QUAT_H
#define QUAT_H

#include <stdbool.h>

#include "mat4.h"
#include "scalar.h"
#include "vec3.h"

typedef struct {
  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };

    float v[4];
  };

} quat;

#define quat_new(X, Y, Z, W) ((quat){.x = X, .y = Y, .z = Z, .w = W})
#define quat_identity() ((quat){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f})

quat quat_mat4_to_quat(mat4 m);
mat4 quat_to_mat4(quat q);
quat quat_look_rotation(vec3 direction, vec3 up);
quat quat_slerp(quat start, quat end, float t);
quat quat_slerp2(quat q1, quat q2, float amount);
quat quat_power(quat q, float f);
quat quat_nlerp(quat from, quat to, float t);
quat quat_mix(quat from, quat to, float t);
vec3 quat_mul_vec3(quat q, vec3 v);
quat quat_mul(quat a, quat b);
quat quat_inverse(quat q);
quat quat_conjugate(quat q);
quat quat_norm(quat q);
float quat_len(quat q);
float quat_len_sq(quat q);
float quat_dot(quat a, quat b);
bool quat_same_orientation(quat a, quat b);
bool quat_not_equal(quat a, quat b);
bool quat_is_equal(quat a, quat b);
quat quat_negate(quat a);
quat quat_scale(quat a, float by);
quat quat_sub(quat a, quat b);
quat quat_add(quat a, quat b);
float quat_get_angle(quat q);
vec3 quat_get_axis(quat q);
quat quat_from_to(vec3 from, vec3 to);
quat quat_angle_axis(float angle, vec3 axis);

#endif // QUAT_H
