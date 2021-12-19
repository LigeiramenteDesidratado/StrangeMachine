#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "mat4.h"
#include "vec3.h"

bool mat4_is_equal(mat4 lhs, mat4 rhs) {

  for (int i = 0; i < 16; i++) {
    if (fabsf(lhs.v[i] - rhs.v[i]) > EPSILON) {
      return false;
    }
  }
  return true;
}

bool mat4_not_equal(mat4 a, mat4 b) {
  bool result = mat4_is_equal(a, b);

  return !result;
}

mat4 mat4_add(mat4 lhs, mat4 rhs) {

  return mat4_new(lhs.xx + rhs.xx, lhs.xy + rhs.xy, lhs.xz + rhs.xz, lhs.xw + rhs.xw, lhs.yx + rhs.yx, lhs.yy + rhs.yy,
                  lhs.yz + rhs.yz, lhs.yw + rhs.yw, lhs.zx + rhs.zx, lhs.zy + rhs.zy, lhs.zz + rhs.zz, lhs.zw + rhs.zw,
                  lhs.tx + rhs.tx, lhs.ty + rhs.ty, lhs.tz + rhs.tz, lhs.tw + rhs.tw);
}

mat4 mat4_scale(mat4 m, float by) {

  return mat4_new(m.xx * by, m.xy * by, m.xz * by, m.xw * by, m.yx * by, m.yy * by, m.yz * by, m.yw * by, m.zx * by,
                  m.zy * by, m.zz * by, m.zw * by, m.tx * by, m.ty * by, m.tz * by, m.tw * by);
}

// dot product on the row and column of matrices a and b
static inline float M4D(const mat4 a, const int aRow, const mat4 b, const int bCol) {
  return a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] +
         a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3];
}

mat4 mat4_mul(mat4 a, mat4 b) {

  return mat4_new(M4D(a, 0, b, 0), M4D(a, 1, b, 0), M4D(a, 2, b, 0), M4D(a, 3, b, 0), M4D(a, 0, b, 1), M4D(a, 1, b, 1),
                  M4D(a, 2, b, 1), M4D(a, 3, b, 1), M4D(a, 0, b, 2), M4D(a, 1, b, 2), M4D(a, 2, b, 2), M4D(a, 3, b, 2),
                  M4D(a, 0, b, 3), M4D(a, 1, b, 3), M4D(a, 2, b, 3), M4D(a, 3, b, 3));
}

// Vector multiplications
static inline float M4V4D(const mat4 m, const int mRow, const float x, const float y, const float z, const float w) {
  return x * m.v[0 * 4 + mRow] + y * m.v[1 * 4 + mRow] + z * m.v[2 * 4 + mRow] + w * m.v[3 * 4 + mRow];
}

vec4 mat4_multiply_vec4(const mat4 m, const vec4 v) {
  return vec4_new(M4V4D(m, 0, v.x, v.y, v.z, v.w), M4V4D(m, 1, v.x, v.y, v.z, v.w), M4V4D(m, 2, v.x, v.y, v.z, v.w),
                  M4V4D(m, 3, v.x, v.y, v.z, v.w));
}

// This function will take vec3 and transform
// it using the provided matrix, assuming the vector represents the direction
// and magnitude.
vec3 mat4_transform_vec3(mat4 m, vec3 v) {

  return vec3_new(M4V4D(m, 0, v.x, v.y, v.z, 0.0f), M4V4D(m, 1, v.x, v.y, v.z, 0.0f), M4V4D(m, 2, v.x, v.y, v.z, 0.0f));
}

// This function will take vec3 and transform
// it using the provided matrix, assuming the vector represents a point
vec3 mat4_transform_point(mat4 m, vec3 v) {

  return vec3_new(M4V4D(m, 0, v.x, v.y, v.z, 1.0f), M4V4D(m, 1, v.x, v.y, v.z, 1.0f), M4V4D(m, 2, v.x, v.y, v.z, 1.0f));
}

mat4 mat4_transpose(mat4 m) {

  return mat4_new(m.xx, m.yx, m.zx, m.tx, m.xy, m.yy, m.zy, m.ty, m.xz, m.yz, m.zz, m.tz, m.xw, m.yw, m.zw, m.tw);
}

// minor determinant of a 3x3 matrix using the given rows and cols
#define M4_3X3MINOR(x, c0, c1, c2, r0, r1, r2)                                                                         \
  (x[c0 * 4 + r0] * (x[c1 * 4 + r1] * x[c2 * 4 + r2] - x[c1 * 4 + r2] * x[c2 * 4 + r1]) -                              \
   x[c1 * 4 + r0] * (x[c0 * 4 + r1] * x[c2 * 4 + r2] - x[c0 * 4 + r2] * x[c2 * 4 + r1]) +                              \
   x[c2 * 4 + r0] * (x[c0 * 4 + r1] * x[c1 * 4 + r2] - x[c0 * 4 + r2] * x[c1 * 4 + r1]))

float mat4_determinant(mat4 m) {

  return m.v[0] * M4_3X3MINOR(m.v, 1, 2, 3, 1, 2, 3) - m.v[4] * M4_3X3MINOR(m.v, 0, 2, 3, 1, 2, 3) +
         m.v[8] * M4_3X3MINOR(m.v, 0, 1, 3, 1, 2, 3) - m.v[12] * M4_3X3MINOR(m.v, 0, 1, 2, 1, 2, 3);
}

mat4 mat4_adjugate(mat4 m) {
  // cofactor(M[i,j]) = Minor(M[i],j] * pow(-1, i+j))
  mat4 cofactor;
  cofactor.v[0] = M4_3X3MINOR(m.v, 1, 2, 3, 1, 2, 3);
  cofactor.v[1] = M4_3X3MINOR(m.v, 1, 2, 3, 0, 2, 3);
  cofactor.v[2] = M4_3X3MINOR(m.v, 1, 2, 3, 0, 1, 3);
  cofactor.v[3] = M4_3X3MINOR(m.v, 1, 2, 3, 0, 1, 2);
  cofactor.v[4] = M4_3X3MINOR(m.v, 0, 2, 3, 1, 2, 3);
  cofactor.v[5] = M4_3X3MINOR(m.v, 0, 2, 3, 0, 2, 3);
  cofactor.v[6] = M4_3X3MINOR(m.v, 0, 2, 3, 0, 1, 3);
  cofactor.v[7] = M4_3X3MINOR(m.v, 0, 2, 3, 0, 1, 2);
  cofactor.v[8] = M4_3X3MINOR(m.v, 0, 1, 3, 1, 2, 3);
  cofactor.v[9] = M4_3X3MINOR(m.v, 0, 1, 3, 0, 2, 3);
  cofactor.v[10] = M4_3X3MINOR(m.v, 0, 1, 3, 0, 1, 3);
  cofactor.v[11] = M4_3X3MINOR(m.v, 0, 1, 3, 0, 1, 2);
  cofactor.v[12] = M4_3X3MINOR(m.v, 0, 1, 2, 1, 2, 3);
  cofactor.v[13] = M4_3X3MINOR(m.v, 0, 1, 2, 0, 2, 3);
  cofactor.v[14] = M4_3X3MINOR(m.v, 0, 1, 2, 0, 1, 3);
  cofactor.v[15] = M4_3X3MINOR(m.v, 0, 1, 2, 0, 1, 2);
  return mat4_transpose(cofactor);
}

mat4 mat4_inverse(mat4 m) {

  float det = mat4_determinant(m);
  if (det == 0.0f) {
    // TODO error
    return mat4_identity();
  }
  mat4 adj = mat4_adjugate(m);
  return mat4_scale(adj, (1.0f / det));
}

mat4 mat4_frustum(float l, float r, float b, float t, float n, float f) {

  if (l == r || t == b || n == f) {
    // TODO error
    return mat4_identity();
  }
  return mat4_new((2.0f * n) / (r - l) , 0                    , 0                         , 0 ,
                  0                    , (2.0f * n) / (t - b) , 0                         , 0 ,
                  (r + l) / (r - l)    , (t + b) / (t - b)    , (-(f + n)) / (f - n)      , -1,
                  0                    , 0                    , (-2.0f * f * n) / (f - n) , 0);
}

mat4 mat4_perspective(float fovDegrees, float aspect, float near, float far) {
  float yMax = near * tanf(fovDegrees * 3.14159265359f / 360.0f);
  float xMax = yMax * aspect;
  return mat4_frustum(-xMax, xMax, -yMax, yMax, near, far);
}

mat4 mat4_ortho(float l, float r, float b, float t, float n, float f) {
  if (l == r || t == b || n == f) {
    // TODO error
    return mat4_identity();
  }

  float rl = r - l;
  float tb = t - b;
  float fn = f - n;
  return mat4_new(2.0f / rl, 0.0f,      0.0f,                         0.0f,
                  0.0f,      2.0f / tb, 0.0f,                         0.0f,
                  0.0f,      0.0f,      -2.0f / fn,                   0.0f,
                  -(l + r) / rl,        -(t + b) / tb, -(f + n) / fn, 1.0f);
}

mat4 mat4_look_at(vec3 position, vec3 target, vec3 up) {
  vec3 f = vec3_scale(vec3_norm(vec3_sub(target, position)), -1.0f);
  vec3 r = vec3_cross(up, f); // right handed
  if (vec3_is_equal(r, vec3_zero())) {
    return mat4_identity();
  }

  r = vec3_norm(r);
  vec3 u = vec3_norm(vec3_cross(f, r));

  vec3 t = vec3_new(-vec3_dot(r, position), -vec3_dot(u, position), -vec3_dot(f, position));

  mat4 result = mat4_new(
      // transpose upper 3x3 matrix to invert it
      r.x, u.x, f.x, 0, r.y, u.y, f.y, 0, r.z, u.z, f.z, 0, t.x, t.y, t.z, 1);

  return result;
}
