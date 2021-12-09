#include "smShapes.h"
#include "util/common.h"

bool check_collision_bounding_box(bounding_box_s bb1, bounding_box_s bb2) {
  bool collision = true;

  if ((bb1.max.x >= bb2.min.x) && (bb1.min.x <= bb2.max.x)) {
    if ((bb1.max.y < bb2.min.y) || (bb1.min.y > bb2.max.y))
      collision = false;
    if ((bb1.max.z < bb2.min.z) || (bb1.min.z > bb2.max.z))
      collision = false;
  } else
    collision = false;

  return collision;
}

// helper
capsule_s shapes_capsule_new(sphere_s s, float height) {

  return (capsule_s){
      .base = vec3_new(s.center.x, s.center.y - s.radius, s.center.z),
      .tip = vec3_new(s.center.x, (s.center.y - s.radius) + height, s.center.z),
      .radius = s.radius,
  };
}

bounding_box_s shapes_get_aabb_sphere(sphere_s s) {
  bounding_box_s result;

  vec3 smin = vec3_sub_val(s.center, s.radius);
  vec3 smax = vec3_add_val(s.center, s.radius);

  result.min = smin;
  result.max = smax;

  return result;
}

bounding_box_s shapes_get_aabb_capsule(capsule_s c) {

  bounding_box_s result;

  vec3 bmin = vec3_sub_val(c.base, c.radius);
  vec3 bmax = vec3_add_val(c.base, c.radius);

  vec3 tmin = vec3_sub_val(c.tip, c.radius);
  vec3 tmax = vec3_add_val(c.tip, c.radius);

  // merge aabb
  vec3 cmin = vec3_min(bmin, tmin);
  vec3 cmax = vec3_max(bmax, tmax);

  result.min = cmin;
  result.max = cmax;

  return result;
}

bounding_box_s shapes_get_aabb_triangle(triangle_s t) {

  bounding_box_s result;

  vec3 tmin = vec3_min(t.p0, vec3_min(t.p2, t.p1));
  vec3 tmax = vec3_max(t.p0, vec3_max(t.p2, t.p1));

  result.min = tmin;
  result.max = tmax;

  return result;
}

