#ifndef SM_SHAPES_H
#define SM_SHAPES_H

#include <stdbool.h>

#include "math/smMath.h"

typedef struct {
  vec3 center;
  float radius;

} sphere_s;

typedef struct {
  vec3 p0;
  vec3 p1;
  vec3 p2;

} triangle_s;

typedef struct {
  vec3 base;
  vec3 tip;
  float radius;

} capsule_s;

typedef struct {
  vec3 min;
  vec3 max;

} bounding_box_s;

bool check_collision_bounding_box(bounding_box_s bb1, bounding_box_s bb2);
capsule_s shapes_capsule_new(sphere_s s, float height);
bounding_box_s shapes_get_aabb_sphere(sphere_s s);
bounding_box_s shapes_get_aabb_capsule(capsule_s c);
bounding_box_s shapes_get_aabb_triangle(triangle_s t);

#endif // SM_SHAPES_H
