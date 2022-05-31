#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "cglm/cglm.h"

#define EPSILON 0.000001f

typedef struct sm__transform_s {
  vec3 position;
  versor rotation;
  vec3 scale;

} sm_transform_s;

#define sm_transform_new(POSITION, ROTATION, SCALE)                                                                    \
  ((sm_transform_s){.position = {POSITION[0], POSITION[1], POSITION[2]},                                               \
                    .rotation = {ROTATION[0], ROTATION[1], ROTATION[2], ROTATION[3]},                                  \
                    .scale = {                                                                                         \
                        SCALE[0],                                                                                      \
                        SCALE[1],                                                                                      \
                        SCALE[2],                                                                                      \
                    }})

/* transform_t transform_new(vec3 p, quat r, vec3 s) { */
/*   return (transform_t){.position = p, .rotation = r, .scale = s}; */
/* } */

/* transform_t transform_zero() { */
/*   return transform_new(vec3_zero(), quat_identity(), */
/*                        vec3_new(1.0f, 1.0f, 1.0f)); */
/* } */
/*  */
#define transform_zero()     ((sm_transform_s){{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}})
#define transform_identity() (transform_new(vec3_zero(), quat_identity(), vec3_new(1.0f, 1.0f, 1.0f)))

#define transform_print(T)                                                                                             \
  printf("%s:\n", #T);                                                                                                 \
  printf("posit: %f, %f, %f\n", T.position[0], T.position[1], T.position[2]);                                          \
  printf("rotat: %f, %f, %f, %f\n", T.rotation[0], T.rotation[1], T.rotation[2], T.rotation[3]);                       \
  printf("scale: %f, %f, %f\n", T.scale[0], T.scale[1], T.scale[2]);

sm_transform_s transform_combine(sm_transform_s a, sm_transform_s b);
void transform_to_mat4(sm_transform_s *t, mat4 out);
sm_transform_s transform_mat4_to_transform(mat4 m);
sm_transform_s transform_inverse(sm_transform_s t);
void transform_point(sm_transform_s a, vec3 b, vec3 out);
void transform_vec3(sm_transform_s a, vec3 b, vec3 out);
sm_transform_s transform_mix(sm_transform_s a, sm_transform_s b, float t);

#endif // TRANSFORM_H
