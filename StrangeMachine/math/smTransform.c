#include "smpch.h"

#include "core/smCore.h"
#include "math/smMath.h"

#include <math.h>

sm_transform_s transform_combine(sm_transform_s a, sm_transform_s b) {
  sm_transform_s out;

  glm_vec3_mul(a.scale.data, b.scale.data, out.scale.data);
  glm_quat_mul(b.rotation.data, a.rotation.data, out.rotation.data);

  sm_vec3 pos;
  glm_vec3_mul(a.scale.data, b.position.data, pos.data);
  glm_quat_rotatev(a.rotation.data, pos.data, out.position.data);
  glm_vec3_add(a.position.data, out.position.data, out.position.data);

  return out;
}

/* sm_transform_s transform_inverse(sm_transform_s t) { */
/*   sm_transform_s inv; */
/**/
/*   glm_quat_inv(t.rotation, inv.rotation); */
/**/
/*   inv.scale[0] = fabs(t.scale[0]) < EPSILON ? 0.0f : 1.0f / t.scale[0]; */
/*   inv.scale[1] = fabs(t.scale[1]) < EPSILON ? 0.0f : 1.0f / t.scale[1]; */
/*   inv.scale[2] = fabs(t.scale[2]) < EPSILON ? 0.0f : 1.0f / t.scale[2]; */
/**/
/*   vec3 inv_trans; */
/*   glm_vec3_scale(t.position, -1.0f, inv_trans); */
/**/
/*   vec3 tmp; */
/*   glm_vec3_mul(inv.scale, inv_trans, tmp); */
/**/
/*   quat_mul_vec3(inv.rotation, tmp, inv.position); */
/**/
/*   return inv; */
/* } */

//  When mixing two transforms together, linearly interpolate the position,
//  rotation, and scale of the input transforms
/* sm_transform_s transform_mix(sm_transform_s a, sm_transform_s b, float t) { */
/**/
/*   versor b_rot; */
/*   glm_quat_copy(b.rotation, b_rot); */
/**/
/*   if (glm_quat_dot(a.rotation, b_rot) < 0.0f) { */
/*     glm_vec4_negate(b_rot); */
/*   } */
/**/
/*   vec3 out_pos; */
/*   glm_vec3_lerp(a.position, b.position, t, out_pos); */
/*   versor out_rot; */
/*   glm_quat_nlerp(a.rotation, b_rot, t, out_rot); */
/*   vec3 out_scale; */
/*   glm_vec3_lerp(a.scale, b.scale, t, out_scale); */
/**/
/*   return transform_new(out_pos, out_rot, out_scale); */
/* } */

void transform_to_mat4(sm_transform_s t, mat4 out) {
  /* first, extract the rotation basis of the transform */
  sm_vec4 x, y, z;
  glm_quat_rotatev(t.rotation.data, sm_vec3_right().data, x.data);
  glm_quat_rotatev(t.rotation.data, sm_vec3_up().data, y.data);
  glm_quat_rotatev(t.rotation.data, sm_vec3_forward().data, z.data);

  /* next, scale the basis vectors */
  glm_vec4_scale(x.data, t.scale.x, x.data);
  glm_vec4_scale(y.data, t.scale.y, y.data);
  glm_vec4_scale(z.data, t.scale.z, z.data);

  sm_mat4 result = sm_mat4_new(x.x, x.y, x.z, 0.0f,                           // X basis (and scale)
                               y.x, y.y, y.z, 0.0f,                           // Y basis (and scale)
                               z.x, z.y, z.z, 0.0f,                           // Z basis (and scale)
                               t.position.x, t.position.y, t.position.z, 1.0f // Position
  );

  glm_mat4_copy(result.data, out);
}

sm_transform_s transform_mat4_to_transform(sm_mat4 m) {
  sm_transform_s out;
  glm_vec3_copy(m.vec3.position.data, out.position.data); // rotation = first column of mat
  glm_mat4_quat(m.data, out.rotation.data);

  /* get the rotate scale matrix, then estimate the scale from that */
  sm_mat4 rot_scale_mat;
  glm_mat4_identity(rot_scale_mat.data);

  rot_scale_mat = sm_mat4_new(m.float16[0], m.float16[1], m.float16[2], 0, m.float16[4], m.float16[5], m.float16[6], 0,
                              m.float16[8], m.float16[9], m.float16[10], 0, 0, 0, 0, 1);

  versor q;
  glm_quat_inv(out.rotation.data, q);
  sm_mat4 inv_rot_mat;
  glm_quat_mat4(q, inv_rot_mat.data);
  sm_mat4 scale_mat;
  glm_mat4_mul(rot_scale_mat.data, inv_rot_mat.data, scale_mat.data);

  /* the diagonal of the scale matrix is the scale */

  out.scale.data[0] = scale_mat.c0r0;
  out.scale.data[1] = scale_mat.c1r1;
  out.scale.data[2] = scale_mat.c2r2;

  /* out.position.data[3] = 1.0f; */
  /* out.scale.data[3] = 1.0f; */

  return out;
}

// void transform_point(sm_transform_s a, vec3 b, vec3 out) {
//   SM_UNUSED(a);
//   SM_UNUSED(b);
//   SM_UNUSED(out);
//   SM_UNIMPLEMENTED(1);
//   /* vec3 out; */
//
//   // Scale -> Rotation -> Translation
//
//   /* out = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b)); */
//   /* out = vec3_add(a.position, out); */
//
//   /* return out; */
// }
//
// void transform_vec3(sm_transform_s a, vec3 b, vec3 out) {
//   SM_UNUSED(a);
//   SM_UNUSED(b);
//   SM_UNUSED(out);
//   SM_UNIMPLEMENTED(1);
//   /* vec3 out; */
//   /* out = quat_mul_vec3(a.rotation, vec3_mul(a.scale, b)); */
//   /* return out; */
// }
