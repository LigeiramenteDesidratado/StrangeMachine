#include <math.h>
#include <stdbool.h>

#include "quat.h"

quat quat_angle_axis(float angle, vec3 axis) {
  vec3 norm = vec3_norm(axis);
  float s = sinf(angle * 0.5f);

  quat result =
      quat_new(norm.x * s, norm.y * s, norm.z * s, cosf(angle * 0.5f));

  return result;
}

quat quat_from_to(vec3 from, vec3 to) {
  vec3 f = vec3_norm(from);
  vec3 t = vec3_norm(to);
  if (vec3_is_equal(f, t)) {
    return quat_identity();

  } else if (vec3_is_equal(f, vec3_scale(t, -1.0f))) {
    vec3 ortho = vec3_new(1.0f, 0.0f, 0.0f);

    if (fabsf(f.y) < fabsf(f.x)) {
      ortho = vec3_new(0.0f, 1.0f, 0.0f);
    }
    if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabs(f.x)) {
      ortho = vec3_new(0.0f, 0.0f, 1.0f);
    }

    vec3 axis = vec3_norm(vec3_cross(f, ortho));

    return quat_new(axis.x, axis.y, axis.z, 0);
  }

  vec3 half = vec3_norm(vec3_add(f, t));
  vec3 axis = vec3_cross(f, half);

  return quat_new(axis.x, axis.y, axis.z, vec3_dot(f, half));
}

vec3 quat_get_axis(quat q) { return vec3_norm(vec3_new(q.x, q.y, q.z)); }

float quat_get_angle(quat q) { return 2.0f * acosf(q.w); }

quat quat_add(quat a, quat b) {
  return quat_new(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

quat quat_sub(quat a, quat b) {
  return quat_new(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

quat quat_scale(quat a, float by) {
  return quat_new(a.x * by, a.y * by, a.z * by, a.w * by);
}

quat quat_negate(quat a) { return quat_new(-a.x, -a.y, -a.z, -a.w); }

bool quat_is_equal(quat a, quat b) {
  return (fabsf(a.x - b.x) <= EPSILON && fabsf(a.y - b.y) <= EPSILON &&
          fabsf(a.z - b.z) <= EPSILON && fabsf(a.w - b.w) <= EPSILON);
}

bool quat_not_equal(quat a, quat b) { return !quat_is_equal(a, b); }

bool quat_same_orientation(quat a, quat b) {
  return (fabsf(a.x - b.x) <= EPSILON && fabsf(a.y - b.y) <= EPSILON &&
          fabsf(a.z - b.z) <= EPSILON && fabsf(a.w - b.w) <= EPSILON) ||
         (fabsf(a.x + b.x) <= EPSILON && fabsf(a.y + b.y) <= EPSILON &&
          fabsf(a.z + b.z) <= EPSILON && fabsf(a.w + b.w) <= EPSILON);
}

float quat_dot(quat a, quat b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float quat_len_sq(quat q) {
  return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float quat_len(quat q) {
  float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;

  if (len_sq < EPSILON) {
    return 0.0f;
  }

  return sqrtf(len_sq);
}

quat quat_norm(quat q) {
  float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
  if (len_sq < EPSILON) {
    return quat_identity();
  }

  float il = 1.0f / sqrtf(len_sq);

  return quat_new(q.x * il, q.y * il, q.z * il, q.w * il);
}

quat quat_conjugate(quat q) { return quat_new(-q.x, -q.y, -q.z, q.w); }

quat quat_inverse(quat q) {
  float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
  if (len_sq < EPSILON) {
    return quat_identity();
  }

  float recip = 1.0f / len_sq;
  return quat_new(-q.x * recip, -q.y * recip, -q.z * recip, q.w * recip);
}

quat quat_mul(quat a, quat b) {
  return quat_new(b.x * a.w + b.y * a.z - b.z * a.y + b.w * a.x,
                  -b.x * a.z + b.y * a.w + b.z * a.x + b.w * a.y,
                  b.x * a.y - b.y * a.x + b.z * a.w + b.w * a.z,
                  -b.x * a.x - b.y * a.y - b.z * a.z + b.w * a.w);
}

vec3 quat_mul_vec3(quat q, vec3 v) {

  vec3 qv = vec3_new(q.x, q.y, q.z);

  vec3 v1 = vec3_scale(vec3_scale(qv, 2.0f), vec3_dot(qv, v));
  vec3 v2 = vec3_scale(v, (q.w * q.w - vec3_dot(qv, qv)));
  vec3 v3 = vec3_scale(vec3_scale(vec3_cross(qv, v), 2.0f), q.w);

  vec3 result = vec3_add(v1, vec3_add(v2, v3));

  return result;
}

// If all input quaternions are of unit length, the resulting quaternion will be
// of unit length as well. This function achieves the same result as lerp does,
// but it's not really a lerp function as the quaternion still travels on an
// arc. To avoid any confusion, this function will be called mix, not lerp. The
// mix function assumes that the input quaternions are in the desired
// neighborhood.
quat quat_mix(quat from, quat to, float t) {

  return quat_add(quat_scale(from, (1.0f - t)), quat_scale(to, t));
}

// Like mix, nlerp also assumes the input vectors are in the desired
// neighborhood.
quat quat_nlerp(quat from, quat to, float t) {

  quat result = quat_add(from, quat_scale(quat_sub(to, from), t));

  return quat_norm(result);
}

quat quat_power(quat q, float f) {
  float angle = 2.0f * acosf(q.w);
  vec3 axis = vec3_norm(vec3_new(q.x, q.y, q.z));

  float halfCos = cosf(f * angle * 0.5f);
  float halfSin = sinf(f * angle * 0.5f);

  return quat_new(axis.x * halfSin, axis.y * halfSin, axis.z * halfSin,
                  halfCos);
}

// slerp should only be used if consistent velocity is required. In most cases,
// nlerp will be a better interpolation method. Depending on the interpolation
// step size, slerp may end
quat quat_slerp(quat start, quat end, float t) {
  if (fabsf(quat_dot(start, end)) > 1.0f - EPSILON) {
    return quat_nlerp(start, end, t);
  }

  quat delta = quat_mul(quat_inverse(start), end);
  return quat_norm(quat_mul(quat_power(delta, t), start));
}

quat quat_slerp2(quat q1, quat q2, float amount) {
  quat result = {0};

  float cosHalfTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

  if (cosHalfTheta < 0) {
    q2.x = -q2.x;
    q2.y = -q2.y;
    q2.z = -q2.z;
    q2.w = -q2.w;
    cosHalfTheta = -cosHalfTheta;
  }

  if (fabs(cosHalfTheta) >= 1.0f)
    result = q1;
  else if (cosHalfTheta > 0.95f)
    result = quat_nlerp(q1, q2, amount);
  else {
    float halfTheta = acosf(cosHalfTheta);
    float sinHalfTheta = sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

    if (fabs(sinHalfTheta) < 0.001f) {
      result.x = (q1.x * 0.5f + q2.x * 0.5f);
      result.y = (q1.y * 0.5f + q2.y * 0.5f);
      result.z = (q1.z * 0.5f + q2.z * 0.5f);
      result.w = (q1.w * 0.5f + q2.w * 0.5f);
    } else {
      float ratioA = sinf((1 - amount) * halfTheta) / sinHalfTheta;
      float ratioB = sinf(amount * halfTheta) / sinHalfTheta;

      result.x = (q1.x * ratioA + q2.x * ratioB);
      result.y = (q1.y * ratioA + q2.y * ratioB);
      result.z = (q1.z * ratioA + q2.z * ratioB);
      result.w = (q1.w * ratioA + q2.w * ratioB);
    }
  }

  return result;
}

quat quat_look_rotation(vec3 direction, vec3 up) {

  // find orhonormal basis vectors
  vec3 f = vec3_norm(direction); // object forward
  vec3 u = vec3_norm(up);        // desired up
  vec3 r = vec3_cross(u, f);     // object right

  u = vec3_cross(f, r); // object up

  // from world forward to object forward
  quat world_to_obj = quat_from_to(vec3_new(0, 0, 1), f);

  // what direction is the new object up?
  vec3 object_up = quat_mul_vec3(world_to_obj, vec3_new(0, 1, 0));

  // from object up to desired up
  quat u2u = quat_from_to(object_up, u);

  // rotate to forward direction first
  // then twist to correct up
  quat result = quat_mul(world_to_obj, u2u);

  // don't forget to normalize the result
  return quat_norm(result);
}

mat4 quat_to_mat4(quat q) {

  vec3 r = quat_mul_vec3(q, world_right);
  vec3 u = quat_mul_vec3(q, world_up);
  vec3 f = quat_mul_vec3(q, world_forward);

  mat4 result = mat4_new(r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0,
                         0, 0, 0, 1);

  return result;
}

quat quat_mat4_to_quat(mat4 m) {

  vec3 up = vec3_norm(vec3_new(m.vectors.up.x, m.vectors.up.y, m.vectors.up.z));
  vec3 forward = vec3_norm(
      vec3_new(m.vectors.forward.x, m.vectors.forward.y, m.vectors.forward.z));

  vec3 right = vec3_cross(up, forward);
  up = vec3_cross(forward, right);

  return quat_look_rotation(forward, up);
}
