#include "util/common.h"

#include "smShapes.h"

bool check_collision_bounding_box(bounding_box_s bb1, bounding_box_s bb2) {
  bool collision = true;

  if ((bb1.max[0] >= bb2.min[0]) && (bb1.min[0] <= bb2.max[0])) {
    if ((bb1.max[1] < bb2.min[1]) || (bb1.min[1] > bb2.max[1]))
      collision = false;
    if ((bb1.max[2] < bb2.min[2]) || (bb1.min[2] > bb2.max[2]))
      collision = false;
  } else
    collision = false;

  return collision;
}

// helper
capsule_s shapes_capsule_new(sphere_s s, float height) {

  capsule_s c;
  glm_vec3_copy(vec3_new(s.center[0], s.center[1] - s.radius, s.center[2]), c.base);
  glm_vec3_copy(vec3_new(s.center[0], (s.center[1] - s.radius) + height, s.center[2]), c.tip);
  c.radius = s.radius;

  return c;
}

bounding_box_s shapes_get_aabb_sphere(sphere_s s) {
  bounding_box_s result;

  vec3 smin;
  smin[0] = s.center[0] - s.radius;
  smin[1] = s.center[1] - s.radius;
  smin[2] = s.center[2] - s.radius;

  vec3 smax;
  smax[0] = s.center[0] + s.radius;
  smax[1] = s.center[1] + s.radius;
  smax[2] = s.center[2] + s.radius;

  glm_vec3_copy(smin, result.min);
  glm_vec3_copy(smax, result.max);

  return result;
}

bounding_box_s shapes_get_aabb_capsule(capsule_s c) {

  bounding_box_s result;

  vec3 bmin;
  bmin[0] = c.base[0] - c.radius;
  bmin[1] = c.base[1] - c.radius;
  bmin[2] = c.base[2] - c.radius;

  vec3 bmax;
  bmax[0] = c.base[0] + c.radius;
  bmax[1] = c.base[1] + c.radius;
  bmax[2] = c.base[2] + c.radius;

  vec3 tmin;
  tmin[0] = c.tip[0] - c.radius;
  tmin[1] = c.tip[1] - c.radius;
  tmin[2] = c.tip[2] - c.radius;

  vec3 tmax;
  tmax[0] = c.tip[0] + c.radius;
  tmax[1] = c.tip[1] + c.radius;
  tmax[2] = c.tip[2] + c.radius;

  vec3 cmin, cmax;
  glm_vec3_minv(bmin, tmin, cmin);
  glm_vec3_maxv(bmax, tmax, cmax);

  glm_vec3_copy(cmin, result.min);
  glm_vec3_copy(cmax, result.max);

  return result;
}

bounding_box_s shapes_get_aabb_triangle(triangle_s t) {

  bounding_box_s result;

  vec3 tmin;
  glm_vec3_minv(t.p2, t.p1, tmin);
  glm_vec3_minv(t.p0, tmin, tmin);

  vec3 tmax;
  glm_vec3_maxv(t.p2, t.p1, tmax);
  glm_vec3_maxv(t.p0, tmax, tmax);

  glm_vec3_copy(tmin, result.min);
  glm_vec3_copy(tmax, result.max);

  return result;
}
