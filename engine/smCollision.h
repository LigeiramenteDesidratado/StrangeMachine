#ifndef SM_COLLISION_H
#define SM_COLLISION_H

#include "smMem.h"
#include "smMesh.h"
#include "smShapes.h"

typedef struct {
  bool valid;
  vec3 position, normal;
  float depth;

} intersect_result_s;

void collision_check_capsules(capsule_s a, capsule_s b, intersect_result_s *result);
void collision_check_sphere_triangle(sphere_s s, triangle_s t, intersect_result_s *result);
void collision_check_capsule_triangle(capsule_s c, triangle_s t, intersect_result_s *result);
void collision_check_spheres(sphere_s a, sphere_s b, intersect_result_s *result);

#endif // SM_COLLISION_H
