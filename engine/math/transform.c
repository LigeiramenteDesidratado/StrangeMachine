#include "transform.h"
#include "mat4.h"
#include "quat.h"
#include <math.h>

transform_s transform_combine(transform_s a, transform_s b) {
  transform_s out;

  out.scale = vec3_mul(a.scale, b.scale);
  out.rotation = quat_mul(b.rotation, a.rotation);

  out.position = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b.position));
  out.position = vec3_add(a.position, out.position);

  return out;
}

transform_s transform_inverse(transform_s t) {
  transform_s inv;

  inv.rotation = quat_inverse(t.rotation);

  inv.scale.x = fabs(t.scale.x) < EPSILON ? 0.0f : 1.0f / t.scale.x;
  inv.scale.y = fabs(t.scale.y) < EPSILON ? 0.0f : 1.0f / t.scale.y;
  inv.scale.z = fabs(t.scale.z) < EPSILON ? 0.0f : 1.0f / t.scale.z;

  vec3 inv_trans = vec3_scale(t.position, -1.0f);

  inv.position = quat_mul_vec3(inv.rotation, vec3_mul(inv.scale, inv_trans));

  return inv;
}

//  When mixing two transforms together, linearly interpolate the position,
//  rotation, and scale of the input transforms
transform_s transform_mix(transform_s a, transform_s b, float t) {
  quat b_rot = b.rotation;
  if (quat_dot(a.rotation, b_rot) < 0.0f) {
    b_rot = quat_negate(b_rot);
  }

  return transform_new(vec3_lerp(a.position, b.position, t),
                       quat_nlerp(a.rotation, b_rot, t),
                       vec3_lerp(a.scale, b.scale, t));
}

mat4 transform_to_mat4(transform_s t) {
  // first, extract the rotation basis of the transform
  vec3 x = quat_mul_vec3(t.rotation, world_right);
  vec3 y = quat_mul_vec3(t.rotation, world_up);
  vec3 z = quat_mul_vec3(t.rotation, world_forward);

  // next, scale the basis vectors
  x = vec3_scale(x, t.scale.x);
  y = vec3_scale(y, t.scale.y);
  z = vec3_scale(z, t.scale.z);

  // extract the position of the transform
  vec3 p = t.position;

  mat4 result = mat4_new(x.x, x.y, x.z, 0.0f, // X basis (and scale)
                         y.x, y.y, y.z, 0.0f, // Y basis (and scale)
                         z.x, z.y, z.z, 0.0f, // Z basis (and scale)
                         p.x, p.y, p.z, 1.0f  // Position
  );

  return result;
}

transform_s transform_mat4_to_transform(mat4 m) {
  transform_s out;
  out.position =
      vec3_new(m.v[12], m.v[13], m.v[14]); // position = last column of mat
  out.rotation = quat_mat4_to_quat(m);

  // get the rotate scale matrix, then estimate the scale from that
  mat4 rotScaleMat = mat4_new(m.v[0], m.v[1], m.v[2], 0, m.v[4], m.v[5], m.v[6],
                              0, m.v[8], m.v[9], m.v[10], 0, 0, 0, 0, 1);
  mat4 invRotMat = quat_to_mat4(quat_inverse(out.rotation));
  mat4 scaleSkewMat = mat4_mul(rotScaleMat, invRotMat);
  out.scale =
      vec3_new(scaleSkewMat.v[0], // the diagonal of the matrix is the scale
               scaleSkewMat.v[5], scaleSkewMat.v[10]);
  return out;
}

vec3 transform_point(transform_s a, vec3 b) {
  vec3 out;

  // Scale -> Rotation -> Translation

  out = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b));
  out = vec3_add(a.position, out);

  return out;
}

vec3 transform_vec3(transform_s a, vec3 b) {
  vec3 out;

  out = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b));

  return out;
}
