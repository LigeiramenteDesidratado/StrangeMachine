#ifndef SM_COLLISION_H
#define SM_COLLISION_H

#include <assert.h>
#include <math.h>

#include "stb_ds/stb_ds.h"

#include "smDebug.h"
#include "smMesh.h"
#include "smMem.h"
#include "smShapes.h"

#define FLT_EPSILON 1.192092896e-07

typedef struct {
  bool valid;
  vec3 position, normal;
  float depth;

} intersect_result_s;

static vec3 __closest_point_on_line_segment(vec3 a, vec3 b, vec3 point);
static void collision_check_capsules(capsule_s a, capsule_s b, intersect_result_s *result);
void collision_check_sphere_triangle(sphere_s s, triangle_s t, intersect_result_s *result);
void collision_check_capsule_triangle(capsule_s c, triangle_s t, intersect_result_s *result);

vec3 __closest_point_on_line_segment(vec3 a, vec3 b, vec3 point) {

  vec3 AB = vec3_sub(b, a);

  float v1 = vec3_dot(vec3_sub(point, a), AB);
  float v2 = vec3_dot(AB, AB);

  float t = v1 / v2;

  float saturate = fminf(fmaxf(t, 0.0f), 1.0f);

  vec3 result = vec3_add(a, vec3_scale(AB, saturate));

  return result;
}

void collision_check_capsules(capsule_s a, capsule_s b, intersect_result_s *result) {
  // capsule A:
  vec3 a_Normal = vec3_norm(vec3_sub(a.tip, a.base));
  vec3 a_LineEndOffset = vec3_scale(a_Normal, a.radius);
  vec3 a_A = vec3_add(a.base, a_LineEndOffset);
  vec3 a_B = vec3_sub(a.tip, a_LineEndOffset);

  // capsule B:
  vec3 b_Normal = vec3_norm(vec3_sub(b.tip, b.base));
  vec3 b_LineEndOffset = vec3_scale(b_Normal, b.radius);
  vec3 b_A = vec3_add(b.base, b_LineEndOffset);
  vec3 b_B = vec3_sub(b.tip, b_LineEndOffset);

  // vectors between line endpoints:
  vec3 v0 = vec3_sub(b_A, a_A);
  vec3 v1 = vec3_sub(b_B, a_A);
  vec3 v2 = vec3_sub(b_A, a_B);
  vec3 v3 = vec3_sub(b_B, a_B);

  // squared distances:
  float d0 = vec3_len_sq(v0);
  float d1 = vec3_len_sq(v1);
  float d2 = vec3_len_sq(v2);
  float d3 = vec3_len_sq(v3);

  // select best potential endpoint on capsule A:
  vec3 bestA;
  if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
    bestA = a_B;
  } else {
    bestA = a_A;
  }

  // select point on capsule B line segment nearest to best potential endpoint
  // on A capsule:
  vec3 bestB = __closest_point_on_line_segment(b_A, b_B, bestA);

  // now do the same for capsule A segment:
  bestA = __closest_point_on_line_segment(a_A, a_B, bestB);

  // Finally, sphere collision:
  vec3 N = vec3_sub(bestA, bestB);
  float len = vec3_len(N);

  N = vec3_div_scalar(N, len);
  vec3 position = vec3_sub(bestA, vec3_scale(N, a.radius));

  float penetration_depth = a.radius + b.radius - len;
  bool intersects = penetration_depth > 0;

  result->position = position;
  result->normal = N;
  result->depth = penetration_depth;
  result->valid = intersects;
}

void collision_check_sphere_triangle(sphere_s s, triangle_s t, intersect_result_s *result) {

  vec3 center = s.center;
  float radius = s.radius;

  vec3 N = vec3_norm(vec3_cross(vec3_sub(t.p1, t.p0), vec3_sub(t.p2, t.p0)));

  // Assert that the triangle is not degenerate.
  assert(vec3_not_equal(N, vec3_zero()));

  // Find the nearest feature on the triangle to the sphere.
  float dist = vec3_dot(vec3_sub(center, t.p0), N);

  // If the center of the sphere is farther from the plane of the triangle
  // than the radius of the sphere, then there cannot be an intersection.
  bool no_intercection = (dist < -radius || dist > radius);

  // Project the center of the sphere onto the plane of the triangle.
  vec3 point0 = vec3_sub(center, vec3_scale(N, dist)); // projected sphere center on triangle plane

  // Compute the cross products of the vector from the base of each edge to
  // the point with each edge vector.
  vec3 c0 = vec3_cross(vec3_sub(point0, t.p0), vec3_sub(t.p1, t.p0));
  vec3 c1 = vec3_cross(vec3_sub(point0, t.p1), vec3_sub(t.p2, t.p1));
  vec3 c2 = vec3_cross(vec3_sub(point0, t.p2), vec3_sub(t.p0, t.p2));

  // If the cross product points in the same direction as the normal the the
  // point is inside the edge (it is zero if is on the edge).
  bool intersection = ((vec3_dot(c0, N) <= 0) & (vec3_dot(c1, N) <= 0)) & (vec3_dot(c2, N) <= 0);

  bool inside = intersection && !no_intercection;

  float radiussq = radius * radius; // sphere radius squared

  // Find the nearest point on each edge.

  // Edge 0,1
  vec3 point1 = __closest_point_on_line_segment(t.p0, t.p1, center);

  // If the distance to the center of the sphere to the point is less than
  // the radius of the sphere then it must intersect.
  float distsq = vec3_len_sq(vec3_sub(center, point1));
  intersection |= distsq <= radiussq;

  // Edge 1,2
  vec3 point2 = __closest_point_on_line_segment(t.p1, t.p2, center);

  // If the distance to the center of the sphere to the point is less than
  // the radius of the sphere then it must intersect.
  distsq = vec3_len_sq(vec3_sub(center, point2));
  intersection |= distsq <= radiussq;

  // Edge 2,0
  vec3 point3 = __closest_point_on_line_segment(t.p2, t.p0, center);

  // If the distance to the center of the sphere to the point is less than
  // the radius of the sphere then it must intersect.
  distsq = vec3_len_sq(vec3_sub(center, point3));
  intersection |= distsq <= radiussq;

  bool intersects = intersection && !no_intercection;

  if (intersects) {

    vec3 bestPoint = point0;

    if (!inside) {
      // If the sphere center's projection on the triangle plane is not
      // within the triangle,
      // determine the closest point on triangle to the sphere center
      float bestDist = vec3_len_sq(vec3_sub(point1, center));
      bestPoint = point1;

      float d = vec3_len_sq(vec3_sub(point2, center));
      if (d < bestDist) {
        bestDist = d;
        bestPoint = point2;
      }

      d = vec3_len_sq(vec3_sub(point3, center));
      if (d < bestDist) {
        bestDist = d;
        bestPoint = point3;
      }
    }

    vec3 intersectionVec = vec3_sub(center, bestPoint);
    float intersectionVecLen = vec3_len(intersectionVec);

    result->valid = true;
    result->depth = radius - intersectionVecLen;
    result->position = bestPoint;
    result->normal = vec3_div_scalar(intersectionVec, intersectionVecLen);
  }
}

void collision_check_capsule_triangle(capsule_s c, triangle_s t, intersect_result_s *result) {

  vec3 base = c.base;
  vec3 tip = c.tip;
  float radius = c.radius;
  vec3 line_end_offset = vec3_scale(vec3_norm(vec3_sub(tip, base)), radius);
  vec3 A = vec3_add(base, line_end_offset);
  vec3 B = vec3_sub(tip, line_end_offset);

  // Compute the plane of the triangle (has to be normalized).
  vec3 N = vec3_norm(vec3_cross(vec3_sub(t.p1, t.p0), vec3_sub(t.p2, t.p0)));

  // Assert that the triangle is not degenerate.
  assert(vec3_not_equal(N, vec3_zero()));

  vec3 reference_point;
  vec3 CapsuleNormal = vec3_norm(vec3_sub(B, A));
  if (fabsf(vec3_dot(N, CapsuleNormal)) < FLT_EPSILON) {
    // Capsule line cannot be intersected with triangle plane (they are
    // parallel)
    //	In this case, just take a point from triangle
    reference_point = t.p0;
  } else {

    // Intersect capsule line with triangle plane:
    vec3 leftop = vec3_sub(base, t.p0);
    float rightop = fabsf(vec3_dot(N, CapsuleNormal));
    float r = vec3_dot(N, vec3_div_scalar(leftop, rightop));

    vec3 line_plane_intersection = vec3_add(vec3_scale(CapsuleNormal, r), base);

    // Compute the cross products of the vector from the base of each edge
    // to the point with each edge vector.
    vec3 c0 = vec3_cross(vec3_sub(line_plane_intersection, t.p0), vec3_sub(t.p1, t.p0));
    vec3 c1 = vec3_cross(vec3_sub(line_plane_intersection, t.p1), vec3_sub(t.p2, t.p1));
    vec3 c2 = vec3_cross(vec3_sub(line_plane_intersection, t.p2), vec3_sub(t.p0, t.p2));

    // If the cross product points in the same direction as the normal the
    // the point is inside the edge (it is zero if is on the edge).
    bool inside = vec3_dot(c0, N) <= 0 && vec3_dot(c1, N) <= 0 && vec3_dot(c2, N) <= 0;

    if (inside) {

      reference_point = line_plane_intersection;
    } else {

      // Edge 1:
      vec3 point1 = __closest_point_on_line_segment(t.p0, t.p1, line_plane_intersection);

      // Edge 2:
      vec3 point2 = __closest_point_on_line_segment(t.p1, t.p2, line_plane_intersection);

      // Edge 3:
      vec3 point3 = __closest_point_on_line_segment(t.p2, t.p0, line_plane_intersection);

      reference_point = point1;
      float best_dist = vec3_len_sq(vec3_sub(point1, line_plane_intersection));
      float d = fabsf(vec3_len_sq(vec3_sub(point2, line_plane_intersection)));
      if (d < best_dist) {
        best_dist = d;
        reference_point = point2;
      }
      d = fabsf(vec3_len_sq(vec3_sub(point3, line_plane_intersection)));
      if (d < best_dist) {
        best_dist = d;
        reference_point = point3;
      }
    }
  }

  // Place a sphere on closest point on line segment to intersection:
  vec3 center = __closest_point_on_line_segment(A, B, reference_point);

  sphere_s sph = {.center = center, .radius = radius};

  collision_check_sphere_triangle(sph, t, result);
}

void collision_check_sphere_mesh(sphere_s s, mesh_s *mesh, intersect_result_s *result) {
  triangle_s t;

  bounding_box_s s_aabb = shapes_get_aabb_sphere(s);

  bounding_box_s mesh_aabb = mesh_get_bounding_box(mesh);
  if (!check_collision_bounding_box(s_aabb, mesh_aabb))
    return;

  for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->vertex.positions); i += 3) {

    t.p0 = mesh->vertex.positions[i + 0];
    t.p1 = mesh->vertex.positions[i + 1];
    t.p2 = mesh->vertex.positions[i + 2];

    bounding_box_s t_aabb = shapes_get_aabb_triangle(t);

    if (!check_collision_bounding_box(s_aabb, t_aabb))
      continue;

    collision_check_sphere_triangle(s, t, result);

    if (result->valid) {
      return;
    }
  }
}

void collision_check_capsule_mesh(capsule_s c, mesh_s *mesh, intersect_result_s *result) {

  triangle_s t;

  bounding_box_s c_aabb = shapes_get_aabb_capsule(c);

  bounding_box_s mesh_aabb = mesh_get_bounding_box(mesh);
  if (!check_collision_bounding_box(c_aabb, mesh_aabb))
    return;

  for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->vertex.positions); i += 3) {

    t.p0 = mesh->vertex.positions[i + 0];
    t.p1 = mesh->vertex.positions[i + 1];
    t.p2 = mesh->vertex.positions[i + 2];

    bounding_box_s t_aabb = shapes_get_aabb_triangle(t);

    if (!check_collision_bounding_box(c_aabb, t_aabb))
      continue;

    collision_check_capsule_triangle(c, t, result);

    if (result->valid) {

      /* DrawSphere(reference_point, 0.1, GREEN); */
      /* DrawLine3D(t.p0, Vector3Add(t.p0, N), RED); */
      /* DrawSphere(t.p0, 0.1, ORANGE); */
      /* debug_draw_line(t.p0, t.p1, vec3_new(1.0f, .6f, .3f)); */
      /* debug_draw_line(t.p1, t.p2, vec3_new(1.0f, .6f, .3f)); */
      /* debug_draw_line(t.p2, t.p0, vec3_new(1.0f, .6f, .3f)); */

      return;
    }
  }
}

#endif // SM_COLLISION_H
