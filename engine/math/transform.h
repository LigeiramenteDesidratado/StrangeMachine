#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "quat.h"
#include "vec3.h"

typedef struct {
  vec3 position;
  quat rotation;
  vec3 scale;

} transform_s;

#define transform_new(POSITION, ROTATION, SCALE)                               \
  ((transform_s){.position = POSITION, .rotation = ROTATION, .scale = SCALE})

/* transform_t transform_new(vec3 p, quat r, vec3 s) { */
/*   return (transform_t){.position = p, .rotation = r, .scale = s}; */
/* } */

/* transform_t transform_zero() { */
/*   return transform_new(vec3_zero(), quat_identity(), */
/*                        vec3_new(1.0f, 1.0f, 1.0f)); */
/* } */
/*  */
#define transform_zero()                                                       \
  (transform_new(vec3_zero(), quat_identity(), vec3_new(1.0f, 1.0f, 1.0f)))

#define transform_print(T) \
  printf("posit: %f, %f, %f\n", T.position.x, T.position.y, T.position.z); \
  printf("rotat: %f, %f, %f, %f\n", T.rotation.x, T.rotation.y, T.rotation.z, T.rotation.w); \
  printf("scale: %f, %f, %f\n", T.scale.x, T.scale.y, T.scale.z); \


transform_s transform_combine(transform_s a, transform_s b);
mat4 transform_to_mat4(transform_s t);
transform_s transform_mat4_to_transform(mat4 m);
transform_s transform_inverse(transform_s t);
vec3 transform_point(transform_s a, vec3 b);
vec3 transform_vec3(transform_s a, vec3 b);
transform_s transform_mix(transform_s a, transform_s b, float t);

#endif // TRANSFORM_H
