#include <math.h>
#include <stdbool.h>

#include "vec3.h"

// Quake III algorithm for fast inverse square root
// Looping 1000000000 times and compiled with the -O3 flag:
//      - rev_sqrt(number)  4.34s user 0.02s system 97% cpu 4.471 total
//      - 1/sqrt(number)    6.33s user 0.03s system 98% cpu 6.483 total
float rev_sqrt(float number) {
  long i;
  float x2, y;
  const float threehalfs = 1.5f;

  x2 = number * 0.5f;
  y = number;
  i = *(long *)&y;
  // I do not understand the following line of code, but I've verified it is
  // required for this function to work correctly
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (threehalfs - (x2 * y * y));

  return y;
}

// Component-wise operations
vec3 vec3_add(vec3 a, vec3 b) {
  vec3 result = vec3_new(a.x + b.x, a.y + b.y, a.z + b.z);

  return result;
}

vec3 vec3_add_val(vec3 a, float b) {
  vec3 result = vec3_new(a.x + b, a.y + b, a.z + b);

  return result;
}

vec3 vec3_sub(vec3 a, vec3 b) {
  vec3 result = vec3_new(a.x - b.x, a.y - b.y, a.z - b.z);

  return result;
}

vec3 vec3_sub_val(vec3 a, float b) {
  vec3 result = vec3_new(a.x - b, a.y - b, a.z - b);

  return result;
}

vec3 vec3_min(vec3 a, vec3 b) {
  vec3 result;
  result.x = fminf(a.x, b.x);
  result.y = fminf(a.y, b.y);
  result.z = fminf(a.z, b.z);

  return result;
}

vec3 vec3_max(vec3 a, vec3 b) {
  vec3 result;

  result.x = fmaxf(a.x, b.x);
  result.y = fmaxf(a.y, b.y);
  result.z = fmaxf(a.z, b.z);

  return result;
}

vec3 vec3_scale(vec3 a, float by) {
  vec3 result = vec3_new(a.x * by, a.y * by, a.z * by);

  return result;
}

vec3 vec3_mul(vec3 a, vec3 b) {
  vec3 result = vec3_new(a.x * b.x, a.y * b.y, a.z * b.z);

  return result;
}

// usefull
vec3 vec3_div_scalar(vec3 a, float by) {

  if (by == 0)
    by = 0.0001f;

  vec3 result = vec3_new(a.x / by, a.y / by, a.z / by);
  return result;
}

// Given two vectors, the dot product returns a scalar value. The result of the
// dot product has the following properties:
// - It is positive if the vectors point in the same direction.
// - It is negative if the vectors point in opposite directions.
// - It is 0 if the vectors are perpendicular.
// If both input vectors have a unit length (you will learn about unit length
// vectors in the Normal vectors section of this chapter), the dot product will
// have a range of -1 to 1.
float vec3_dot(vec3 a, vec3 b) {
  float result = a.x * b.x + a.y * b.y + a.z * b.z;

  return result;
}

// Non-component-wise operations

float vec3_len_sq(vec3 v) {
  float result = v.x * v.x + v.y * v.y + v.z * v.z;

  return result;
}

float vec3_len(vec3 v) {
  float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;

  if (len_sq < EPSILON) {
    return 0.0f;
  }

  return sqrtf(len_sq);
}

vec3 vec3_norm(vec3 v) {

  vec3 result = v;

  float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;

  if (len_sq < EPSILON) {
    return vec3_zero();
  }
  float inv_len = rev_sqrt(len_sq);

  result.x *= inv_len;
  result.y *= inv_len;
  result.z *= inv_len;

  return result;
}

float vec3_angle(vec3 a, vec3 b) {
  float mag_sq_a = a.x * a.x + a.y * a.y + a.z * a.z;
  float mag_sq_b = b.x * b.x + b.y * b.y + b.z * b.z;

  if (mag_sq_a < EPSILON || mag_sq_b < EPSILON) {
    return 0.0f;
  }

  float dot = a.x * b.x + a.y * b.y + a.z * b.z;
  float len = sqrtf(mag_sq_a) * sqrtf(mag_sq_b);
  return acosf(dot / len);
}

void vec3_orthonorm(float left[3], float up[3], const float v[3]) {
  float lenSqr, invLen;
  if (fabs(v[2]) > 0.7f) {
    lenSqr = v[1] * v[1] + v[2] * v[2];
    invLen = rev_sqrt(lenSqr);

    up[0] = 0.0f;
    up[1] = v[2] * invLen;
    up[2] = -v[1] * invLen;

    left[0] = lenSqr * invLen;
    left[1] = -v[0] * up[2];
    left[2] = v[0] * up[1];
  } else {
    lenSqr = v[0] * v[0] + v[1] * v[1];
    invLen = rev_sqrt(lenSqr);

    left[0] = -v[1] * invLen;
    left[1] = v[0] * invLen;
    left[2] = 0.0f;

    up[0] = -v[2] * left[1];
    up[1] = v[2] * left[0];
    up[2] = lenSqr * invLen;
  }
}

// projection of vector a onto vector b
vec3 vec3_proj(vec3 a, vec3 b) {

  float mag_sq_b = vec3_len(b);
  if (mag_sq_b < EPSILON) {
    return vec3_zero();
  }

  float scale = vec3_dot(a, b) / mag_sq_b;
  return vec3_scale(b, scale);
}

// rejection of vector a onto vector b
vec3 vec3_rej(vec3 a, vec3 b) {
  vec3 proj = vec3_proj(a, b);

  return vec3_sub(a, proj);
}

// bounce reflection from the projection of vector a onto vector b
vec3 vec3_ref(vec3 a, vec3 b) {
  float mag_sq_b = vec3_len(b);
  if (mag_sq_b < EPSILON) {
    return vec3_zero();
  }

  float scale = vec3_dot(a, b) / mag_sq_b;
  vec3 proj2 = vec3_scale(b, (scale * 2));

  return vec3_sub(a, proj2);
}

// cross product between vector a and vector b
vec3 vec3_cross(vec3 a, vec3 b) {

  vec3 result = vec3_new(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);

  return result;
}

// linear interpolation between vector a and vector b
vec3 vec3_lerp(vec3 a, vec3 b, float t) {
  vec3 result = vec3_new(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);

  return result;
}

// spherical linear interpolation between vector a and vector b
vec3 vec3_slerp(vec3 a, vec3 b, float t) {
  if (t < 0.01f) {
    return vec3_lerp(a, b, t);
  }

  vec3 from = vec3_norm(a);
  vec3 to = vec3_norm(b);

  float theta = vec3_angle(from, to);
  float sin_theta = sinf(theta);

  float v1 = sinf((1.0f - t) * theta) / sin_theta;
  float v2 = sinf(t * theta) / sin_theta;

  vec3 result = vec3_add(vec3_scale(from, v1), vec3_scale(to, v2));

  return result;
}

vec3 vec3_nlerp(vec3 a, vec3 b, float t) {
  vec3 result = vec3_new(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);

  return vec3_norm(result);
}

// Comparing vectors

bool vec3_is_equal(vec3 a, vec3 b) {
  vec3 diff = vec3_sub(a, b);

  return vec3_len_sq(diff) < EPSILON;
}

bool vec3_not_equal(vec3 a, vec3 b) {
  bool result = vec3_is_equal(a, b);

  return !result;
}
