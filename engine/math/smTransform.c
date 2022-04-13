#include "smpch.h"

#include "cglm/quat.h"
#include "cglm/vec3.h"
#include "cglm/vec4.h"

#include "math/smMath.h"

#include "core/smCore.h"

#include "math/smTransform.h"
#include <math.h>

void quat_mul_vec3(versor q, vec3 v, vec3 out) {

  vec3 qv = {q[0], q[1], q[2]};

  vec3 s;
  glm_vec3_scale(qv, 2.0f, s);

  float by = glm_vec3_dot(qv, v);

  vec3 v1, v2, v3;
  glm_vec3_scale(s, by, v1);
  glm_vec3_scale(v, (q[3] * q[3] - glm_vec3_dot(qv, qv)), v2);
  glm_vec3_cross(qv, v, v3);
  glm_vec3_scale(v3, 2.0f, v3);
  glm_vec3_scale(v3, q[3], v3);

  glm_vec3_add(v2, v3, out);
  glm_vec3_add(v1, out, out);
}

transform_s transform_combine(transform_s a, transform_s b) {
  transform_s out;

  glm_vec3_mul(a.scale, b.scale, out.scale);
  glm_quat_mul(b.rotation, a.rotation, out.rotation);

  vec3 pos;
  glm_vec3_mul(a.scale, b.position, pos);
  quat_mul_vec3(a.rotation, pos, out.position);
  glm_vec3_add(a.position, out.position, out.position);

  return out;
}

transform_s transform_inverse(transform_s t) {
  transform_s inv;

  glm_quat_inv(t.rotation, inv.rotation);

  inv.scale[0] = fabs(t.scale[0]) < EPSILON ? 0.0f : 1.0f / t.scale[0];
  inv.scale[1] = fabs(t.scale[1]) < EPSILON ? 0.0f : 1.0f / t.scale[1];
  inv.scale[2] = fabs(t.scale[2]) < EPSILON ? 0.0f : 1.0f / t.scale[2];

  vec3 inv_trans;
  glm_vec3_scale(t.position, -1.0f, inv_trans);

  vec3 tmp;
  glm_vec3_mul(inv.scale, inv_trans, tmp);

  quat_mul_vec3(inv.rotation, tmp, inv.position);

  return inv;
}

//  When mixing two transforms together, linearly interpolate the position,
//  rotation, and scale of the input transforms
transform_s transform_mix(transform_s a, transform_s b, float t) {

  versor b_rot;
  glm_quat_copy(b.rotation, b_rot);

  if (glm_quat_dot(a.rotation, b_rot) < 0.0f) {
    glm_vec4_negate(b_rot);
  }

  vec3 out_pos;
  glm_vec3_lerp(a.position, b.position, t, out_pos);
  versor out_rot;
  glm_quat_nlerp(a.rotation, b_rot, t, out_rot);
  vec3 out_scale;
  glm_vec3_lerp(a.scale, b.scale, t, out_scale);

  return transform_new(out_pos, out_rot, out_scale);
}

void transform_to_mat4(transform_s t, mat4 out) {
  // first, extract the rotation basis of the transform
  vec3 x, y, z;
  quat_mul_vec3(t.rotation, (vec3){1.0f, 0.0f, 0.0f}, x);
  quat_mul_vec3(t.rotation, (vec3){0.0f, 1.0f, 0.0f}, y);
  quat_mul_vec3(t.rotation, (vec3){0.0f, 0.0f, 1.0f}, z);

  // next, scale the basis vectors
  glm_vec3_scale(x, t.scale[0], x);
  glm_vec3_scale(y, t.scale[1], y);
  glm_vec3_scale(z, t.scale[2], z);

  // extract the position of the transform
  vec3 p;
  glm_vec3_copy(t.position, p);

  // construct the matrix
  out[0][0] = x[0];
  out[0][1] = x[1];
  out[0][2] = x[2];
  out[0][3] = 0.0f;

  out[1][0] = y[0];
  out[1][1] = y[1];
  out[1][2] = y[2];
  out[1][3] = 0.0f;

  out[2][0] = z[0];
  out[2][1] = z[1];
  out[2][2] = z[2];
  out[2][3] = 0.0f;

  out[3][0] = p[0];
  out[3][1] = p[1];
  out[3][2] = p[2];
  out[3][3] = 1.0f;
}

void quat_look_rotation(vec3 direction, vec3 up, versor out) {

  // find orhonormal basis vectors
  vec3 f, u, r;
  glm_vec3_normalize_to(direction, f); // object forward
  glm_vec3_normalize_to(up, u);        // desired up
  glm_vec3_cross(u, f, r);             // object right

  glm_vec3_cross(f, r, u); // object up

  // from world forward to object forward
  versor world_to_obj;
  /* quat_from_to(vec3_new(0, 0, 1), f); */
  glm_quat_from_vecs(vec3_new(0, 0, 1), f, world_to_obj);

  // what direction is the new object up?
  vec3 object_up;
  quat_mul_vec3(world_to_obj, vec3_new(0, 1, 0), object_up);

  // from object up to desired up
  versor u2u;
  glm_quat_from_vecs(object_up, u, u2u);

  // rotate to forward direction first
  // then twist to correct up
  versor result;
  /* glm_quat_mul(world_to_obj, u2u, result); */

  /* glm_quat_init(float *q, float x, float y, float z, float w) */
  glm_quat_init(
      result, u2u[0] * world_to_obj[3] + u2u[1] * world_to_obj[2] - u2u[2] * world_to_obj[1] + u2u[3] * world_to_obj[0],
      -u2u[0] * world_to_obj[2] + u2u[1] * world_to_obj[3] + u2u[2] * world_to_obj[0] + u2u[3] * world_to_obj[1],
      u2u[0] * world_to_obj[1] - u2u[1] * world_to_obj[0] + u2u[2] * world_to_obj[3] + u2u[3] * world_to_obj[2],
      -u2u[0] * world_to_obj[0] - u2u[1] * world_to_obj[1] - u2u[2] * world_to_obj[2] + u2u[3] * world_to_obj[3]);
  /* glm_quat_mul(world_to_obj, u2u, result); */

  // don't forget to normalize the result
  glm_quat_normalize_to(result, out);
}

void quat_mat4_to_quat(mat4 m, versor out) {

  vec3 up = {m[1][0], m[1][1], m[1][2]};
  glm_vec3_normalize(up);
  vec3 fwd = {m[2][0], m[2][1], m[2][2]};
  glm_vec3_normalize(fwd);

  vec3 right;
  glm_vec3_cross(up, fwd, right);
  glm_vec3_cross(fwd, right, up);

  quat_look_rotation(fwd, up, out);

  /* return quat_look_rotation(forward, up); */
}

transform_s transform_mat4_to_transform(mat4 m) {
  transform_s out;
  /* out.position = vec3_new(m.v[12], m.v[13], m.v[14]); // position = last column of mat */
  glm_vec3_copy((vec3){m[3][0], m[3][1], m[3][2]}, out.position); // rotation = first column of mat
  quat_mat4_to_quat(m, out.rotation);

  /* SM_UNIMPLEMENTED(1); */

  // get the rotate scale matrix, then estimate the scale from that
  mat4 rot_scale_mat;
  glm_mat4_identity(rot_scale_mat);
  rot_scale_mat[0][0] = m[0][0];
  rot_scale_mat[0][1] = m[0][1];
  rot_scale_mat[0][2] = m[0][2];
  rot_scale_mat[0][3] = 0.0f;
  rot_scale_mat[1][0] = m[1][0];
  rot_scale_mat[1][1] = m[1][1];
  rot_scale_mat[1][2] = m[1][2];
  rot_scale_mat[1][3] = 0.0f;
  rot_scale_mat[2][0] = m[2][0];
  rot_scale_mat[2][1] = m[2][1];
  rot_scale_mat[2][2] = m[2][2];
  rot_scale_mat[2][3] = 0.0f;
  rot_scale_mat[3][0] = 0.0f;
  rot_scale_mat[3][1] = 0.0f;
  rot_scale_mat[3][2] = 0.0f;
  rot_scale_mat[3][3] = 1.0f;

  versor q;
  glm_quat_inv(out.rotation, q);
  mat4 inv_rot_mat;
  glm_quat_mat4(q, inv_rot_mat);
  mat4 scale_mat;
  glm_mat4_mul(rot_scale_mat, inv_rot_mat, scale_mat);

  // the diagonal of the scale matrix is the scale

  out.scale[0] = scale_mat[0][0];
  out.scale[1] = scale_mat[1][1];
  out.scale[2] = scale_mat[2][2];

  /* mat4_new(m.v[0], m.v[1], m.v[2], 0, m.v[4], m.v[5], m.v[6], 0, m.v[8], m.v[9], m.v[10], 0, 0, 0, 0, 1); */
  /* mat4 invRotMat = quat_to_mat4(quat_inverse(out.rotation)); */
  /* mat4 scaleSkewMat = mat4_mul(rotScaleMat, invRotMat); */
  /* out.scale = vec3_new(scaleSkewMat.v[0], // the diagonal of the matrix is the scale */
  /* scaleSkewMat.v[5], scaleSkewMat.v[10]); */
  return out;
}

void transform_point(transform_s a, vec3 b, vec3 out) {
  SM_UNUSED(a);
  SM_UNUSED(b);
  SM_UNUSED(out);
  SM_UNIMPLEMENTED(1);
  /* vec3 out; */

  // Scale -> Rotation -> Translation

  /* out = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b)); */
  /* out = vec3_add(a.position, out); */

  /* return out; */
}

void transform_vec3(transform_s a, vec3 b, vec3 out) {
  SM_UNUSED(a);
  SM_UNUSED(b);
  SM_UNUSED(out);
  SM_UNIMPLEMENTED(1);
  /* vec3 out; */
  /* out = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b)); */
  /* return out; */
}
