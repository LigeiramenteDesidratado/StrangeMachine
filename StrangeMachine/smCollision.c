#include "data/array.h"

#include <math.h>

#include "math/smMath.h"
#include "smCollision.h"
#include "smDebug.h"
#include "smMesh.h"
#include "smShapes.h"

#define SM_FLT_EPSILON 1.192092896e-07

void __closest_point_on_line_segment(vec3 a, vec3 b, vec3 point, vec3 out) {

  vec3 ab;
  glm_vec3_sub(b, a, ab);

  vec3 ap;
  glm_vec3_sub(point, a, ap);

  float t = glm_vec3_dot(ap, ab) / glm_vec3_dot(ab, ab);

  t = fminf(fmaxf(t, 0.0f), 1.0f);

  vec3 closest_point;
  glm_vec3_scale(ab, t, closest_point);
  glm_vec3_add(a, closest_point, out);
}

// vec3 __closest_point_on_line_segment(vec3 a, vec3 b, vec3 point) {
//
//   vec3 AB = vec3_sub(b, a);
//
//   float v1 = vec3_dot(vec3_sub(point, a), AB);
//   float v2 = vec3_dot(AB, AB);
//
//   float t = v1 / v2;
//
//   float saturate = fminf(fmaxf(t, 0.0f), 1.0f);
//
//   vec3 result = vec3_add(a, vec3_scale(AB, saturate));
//
//   return result;
// }

void collision_check_capsules(capsule_s a, capsule_s b, intersect_result_s *result) {
  // capsule A:
  vec3 a_norm;
  glm_vec3_sub(a.tip, a.base, a_norm);
  glm_vec3_normalize(a_norm);

  vec3 a_line_end_offset;
  glm_vec3_scale(a_norm, a.radius, a_line_end_offset);

  vec3 a_a, a_b;
  glm_vec3_add(a.base, a_line_end_offset, a_a);
  glm_vec3_sub(a.tip, a_line_end_offset, a_b);

  // capsule B:
  vec3 b_norm;
  glm_vec3_sub(b.tip, b.base, b_norm);
  glm_vec3_normalize(b_norm);

  vec3 b_line_end_offset;
  glm_vec3_scale(b_norm, b.radius, b_line_end_offset);

  vec3 b_a, b_b;
  glm_vec3_add(b.base, b_line_end_offset, b_a);
  glm_vec3_sub(b.tip, b_line_end_offset, b_b);

  // vectors between line endpoints:
  vec3 v0, v1, v2, v3;
  glm_vec3_sub(b_a, a_a, v0);
  glm_vec3_sub(b_b, a_a, v1);
  glm_vec3_sub(b_a, a_b, v2);
  glm_vec3_sub(b_b, a_b, v3);

  // squared distances
  float d0 = glm_vec3_norm2(v0);
  float d1 = glm_vec3_norm2(v1);
  float d2 = glm_vec3_norm2(v2);
  float d3 = glm_vec3_norm2(v3);

  // select best potential endpoint on capsule A:
  vec3 a_best_point;
  if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
    glm_vec3_copy(a_b, a_best_point);
  } else {
    glm_vec3_copy(a_a, a_best_point);
  }

  // select point on capsule B line segment nearest to best potential endpoint
  // on A capsule:
  vec3 b_best_point;
  __closest_point_on_line_segment(b_a, b_b, a_best_point, b_best_point);

  // now do the same for capsule A segment:
  __closest_point_on_line_segment(a_a, a_b, b_best_point, a_best_point);

  // Finally, sphere collision:
  vec3 v;
  glm_vec3_sub(a_best_point, b_best_point, v);
  float d = glm_vec3_norm(v); // vector length

  /* vec3 N = vec3_sub(bestA, bestB); */
  /* float len = vec3_len(N); */

  glm_vec3_divs(v, d, v);
  vec3 position;
  glm_vec3_scale(v, a.radius, position);
  glm_vec3_sub(a_best_point, position, position);

  float depth = a.radius + b.radius - d;

  glm_vec3_copy(v, result->normal);
  glm_vec3_copy(position, result->position);

  result->valid = depth > 0.0f;
  result->depth = depth;
}

// void collision_check_capsules(capsule_s a, capsule_s b, intersect_result_s *result) {
//   // capsule A:
//   vec3 a_Normal = vec3_norm(vec3_sub(a.tip, a.base));
//   vec3 a_LineEndOffset = vec3_scale(a_Normal, a.radius);
//   vec3 a_A = vec3_add(a.base, a_LineEndOffset);
//   vec3 a_B = vec3_sub(a.tip, a_LineEndOffset);
//
//   // capsule B:
//   vec3 b_Normal = vec3_norm(vec3_sub(b.tip, b.base));
//   vec3 b_LineEndOffset = vec3_scale(b_Normal, b.radius);
//   vec3 b_A = vec3_add(b.base, b_LineEndOffset);
//   vec3 b_B = vec3_sub(b.tip, b_LineEndOffset);
//
//   // vectors between line endpoints:
//   vec3 v0 = vec3_sub(b_A, a_A);
//   vec3 v1 = vec3_sub(b_B, a_A);
//   vec3 v2 = vec3_sub(b_A, a_B);
//   vec3 v3 = vec3_sub(b_B, a_B);
//
//   // squared distances:
//   float d0 = vec3_len_sq(v0);
//   float d1 = vec3_len_sq(v1);
//   float d2 = vec3_len_sq(v2);
//   float d3 = vec3_len_sq(v3);
//
//   // select best potential endpoint on capsule A:
//   vec3 bestA;
//   if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
//     bestA = a_B;
//   } else {
//     bestA = a_A;
//   }
//
//   // select point on capsule B line segment nearest to best potential endpoint
//   // on A capsule:
//   vec3 bestB = __closest_point_on_line_segment(b_A, b_B, bestA);
//
//   // now do the same for capsule A segment:
//   bestA = __closest_point_on_line_segment(a_A, a_B, bestB);
//
//   // Finally, sphere collision:
//   vec3 N = vec3_sub(bestA, bestB);
//   float len = vec3_len(N);
//
//   N = vec3_div_scalar(N, len);
//   vec3 position = vec3_sub(bestA, vec3_scale(N, a.radius));
//
//   float penetration_depth = a.radius + b.radius - len;
//   bool intersects = penetration_depth > 0;
//
//   result->position = position;
//   result->normal = N;
//   result->depth = penetration_depth;
//   result->valid = intersects;
// }

void collision_check_sphere_triangle(sphere_s s, triangle_s t, intersect_result_s *result) {

  /* vec3 center = s.center; */
  float radius = s.radius;

  vec3 v1, v2;
  glm_vec3_sub(t.p1, t.p0, v1);
  glm_vec3_sub(t.p2, t.p0, v2);

  vec3 n;
  glm_vec3_cross(v1, v2, n);
  glm_vec3_normalize(n);

  /* assert that the triangle is not degenerate. */
  SM_ASSERT(glm_vec3_norm2(n) > 0.0f);

  /* Find the nearest feature on the triangle to the sphere. */
  vec3 sub;
  glm_vec3_sub(s.center, t.p0, sub);
  float d = glm_vec3_dot(sub, n);

  /* If the center of the sphere is farther from the plane of the triangle */
  /* than the radius of the sphere, then there cannot be an intersection. */
  bool no_intersection = (d < -radius || d > radius);

  /* Project the center of the sphere onto the plane of the triangle. */
  vec3 point0;
  glm_vec3_scale(n, d, point0);
  glm_vec3_sub(s.center, point0, point0); /* projected sphere center on triangle plane */

  /* Compute the cross products of the vector from the base of each edge to */
  /* the point with each edge vector. */
  vec3 edge0, edge1, edge2;
  glm_vec3_sub(t.p1, t.p0, edge0);
  glm_vec3_sub(t.p2, t.p1, edge1);
  glm_vec3_sub(t.p0, t.p2, edge2);

  vec3 p0, p1, p2;
  glm_vec3_sub(point0, t.p0, p0);
  glm_vec3_sub(point0, t.p1, p1);
  glm_vec3_sub(point0, t.p2, p2);

  vec3 c0, c1, c2;
  glm_vec3_cross(p0, edge0, c0);
  glm_vec3_cross(p1, edge1, c1);
  glm_vec3_cross(p2, edge2, c2);

  // If the cross product points in the same direction as the normal the the
  // point is inside the edge (it is zero if is on the edge).
  // TODO: investigate if this is correct.
  bool intersection = ((glm_vec3_dot(c0, n) <= 0) & (glm_vec3_dot(c1, n) <= 0)) & (glm_vec3_dot(c2, n) <= 0);

  bool inside = intersection && !no_intersection;

  float radiussq = radius * radius; // sphere radius squared

  // Find the nearest point on each edge.

  // Edge 0,1
  vec3 point1;
  __closest_point_on_line_segment(t.p0, t.p1, s.center, point1);

  // If the distance to the center of the sphere to the point is less than
  // the radius of the sphere then it must intersect.
  vec3 res1;
  glm_vec3_sub(s.center, point1, res1);
  float distsq = glm_vec3_norm2(res1);
  intersection |= distsq <= radiussq;

  // Edge 1,2
  vec3 point2;
  __closest_point_on_line_segment(t.p1, t.p2, s.center, point2);

  // If the distance to the center of the sphere to the point is less than
  // the radius of the sphere then it must intersect.
  glm_vec3_sub(s.center, point2, res1);
  distsq = glm_vec3_norm2(res1);
  intersection |= distsq <= radiussq;

  // Edge 2,0
  vec3 point3;
  __closest_point_on_line_segment(t.p2, t.p0, s.center, point3);

  // If the distance to the center of the sphere to the point is less than
  // the radius of the sphere then it must intersect.
  glm_vec3_sub(s.center, point3, res1);
  distsq = glm_vec3_norm2(res1);
  intersection |= distsq <= radiussq;

  bool intersects = intersection && !no_intersection;

  if (intersects) {
    vec3 best_point;
    glm_vec3_copy(point0, best_point);

    if (!inside) {
      // If the sphere center's projection on the triangle plane is not
      // within the triangle,
      // determine the closest point on triangle to the sphere center
      glm_vec3_sub(point1, s.center, res1);
      float best_dist = glm_vec3_norm2(res1);
      glm_vec3_copy(point1, best_point);

      glm_vec3_sub(point2, s.center, res1);
      float d = glm_vec3_norm2(res1);

      if (d < best_dist) {
        glm_vec3_copy(point2, best_point);
        best_dist = d;
      }

      glm_vec3_sub(point3, s.center, res1);
      d = glm_vec3_norm2(res1);
      if (d < best_dist) {
        glm_vec3_copy(point3, best_point);
        best_dist = d;
      }
    }

    vec3 intersection_vector;
    glm_vec3_sub(s.center, best_point, intersection_vector);
    float intersection_length = glm_vec3_norm(intersection_vector);

    result->valid = true;
    result->depth = radius - intersection_length;
    glm_vec3_copy(best_point, result->position);
    glm_vec3_divs(intersection_vector, intersection_length, result->normal);
  }
}

// void collision_check_sphere_triangle(sphere_s s, triangle_s t, intersect_result_s *result) {
//
//   vec3 center = s.center;
//   float radius = s.radius;
//
//   vec3 N = vec3_norm(vec3_cross(vec3_sub(t.p1, t.p0), vec3_sub(t.p2, t.p0)));
//
//   // SM_ASSERT that the triangle is not degenerate.
//   SM_ASSERT(vec3_not_equal(N, vec3_zero()));
//
//   // Find the nearest feature on the triangle to the sphere.
//   float dist = vec3_dot(vec3_sub(center, t.p0), N);
//
//   // If the center of the sphere is farther from the plane of the triangle
//   // than the radius of the sphere, then there cannot be an intersection.
//   bool no_intercection = (dist < -radius || dist > radius);
//
//   // Project the center of the sphere onto the plane of the triangle.
//   vec3 point0 = vec3_sub(center, vec3_scale(N, dist)); // projected sphere center on triangle plane
//
//   // Compute the cross products of the vector from the base of each edge to
//   // the point with each edge vector.
//   vec3 c0 = vec3_cross(vec3_sub(point0, t.p0), vec3_sub(t.p1, t.p0));
//   vec3 c1 = vec3_cross(vec3_sub(point0, t.p1), vec3_sub(t.p2, t.p1));
//   vec3 c2 = vec3_cross(vec3_sub(point0, t.p2), vec3_sub(t.p0, t.p2));
//
//   // If the cross product points in the same direction as the normal the the
//   // point is inside the edge (it is zero if is on the edge).
//   bool intersection = ((vec3_dot(c0, N) <= 0) & (vec3_dot(c1, N) <= 0)) & (vec3_dot(c2, N) <= 0);
//
//   bool inside = intersection && !no_intercection;
//
//   float radiussq = radius * radius; // sphere radius squared
//
//   // Find the nearest point on each edge.
//
//   // Edge 0,1
//   vec3 point1 = __closest_point_on_line_segment(t.p0, t.p1, center);
//
//   // If the distance to the center of the sphere to the point is less than
//   // the radius of the sphere then it must intersect.
//   float distsq = vec3_len_sq(vec3_sub(center, point1));
//   intersection |= distsq <= radiussq;
//
//   // Edge 1,2
//   vec3 point2 = __closest_point_on_line_segment(t.p1, t.p2, center);
//
//   // If the distance to the center of the sphere to the point is less than
//   // the radius of the sphere then it must intersect.
//   distsq = vec3_len_sq(vec3_sub(center, point2));
//   intersection |= distsq <= radiussq;
//
//   // Edge 2,0
//   vec3 point3 = __closest_point_on_line_segment(t.p2, t.p0, center);
//
//   // If the distance to the center of the sphere to the point is less than
//   // the radius of the sphere then it must intersect.
//   distsq = vec3_len_sq(vec3_sub(center, point3));
//   intersection |= distsq <= radiussq;
//
//   bool intersects = intersection && !no_intercection;
//
//   if (intersects) {
//
//     vec3 bestPoint = point0;
//
//     if (!inside) {
//       // If the sphere center's projection on the triangle plane is not
//       // within the triangle,
//       // determine the closest point on triangle to the sphere center
//       float bestDist = vec3_len_sq(vec3_sub(point1, center));
//       bestPoint = point1;
//
//       float d = vec3_len_sq(vec3_sub(point2, center));
//       if (d < bestDist) {
//         bestDist = d;
//         bestPoint = point2;
//       }
//
//       d = vec3_len_sq(vec3_sub(point3, center));
//       if (d < bestDist) {
//         bestDist = d;
//         bestPoint = point3;
//       }
//     }
//
//     vec3 intersectionVec = vec3_sub(center, bestPoint);
//     float intersectionVecLen = vec3_len(intersectionVec);
//
//     result->valid = true;
//     result->depth = radius - intersectionVecLen;
//     result->position = bestPoint;
//     result->normal = vec3_div_scalar(intersectionVec, intersectionVecLen);
//   }
// }
//

void collision_check_capsule_triangle(capsule_s c, triangle_s t, intersect_result_s *result) {

  vec3 base, tip;
  glm_vec3_copy(c.base, base);
  glm_vec3_copy(c.tip, tip);
  float radius = c.radius;

  vec3 line_end_offset;
  glm_vec3_sub(tip, base, line_end_offset);
  glm_vec3_normalize(line_end_offset);
  glm_vec3_scale(line_end_offset, radius, line_end_offset);

  vec3 a, b;
  glm_vec3_add(base, line_end_offset, a);
  glm_vec3_sub(tip, line_end_offset, b);

  // Compute the plane of the triangle (has to be normalized).
  vec3 n, v1, v2;
  glm_vec3_sub(t.p1, t.p0, v1);
  glm_vec3_sub(t.p2, t.p0, v2);
  glm_vec3_cross(v1, v2, n);
  glm_vec3_normalize(n);

  // assert that the triangle is not degenerate.
  SM_ASSERT(glm_vec3_norm2(n) > 0.0f);

  vec3 reference_point;
  vec3 capsule_normal;
  glm_vec3_sub(b, a, capsule_normal);
  glm_vec3_normalize(capsule_normal);

  if (fabsf(glm_vec3_dot(n, capsule_normal)) < SM_FLT_EPSILON) {
    /* Capsule line cannot be intersected with triangle plane (they are */
    /* parallel) */
    /* In this case, just take a point from triangle */
    glm_vec3_copy(t.p0, reference_point);
  } else {
    /* Intersect capsule line with triangle plane: */
    vec3 left_op;
    glm_vec3_sub(base, t.p0, left_op);
    float right_op = fabsf(glm_vec3_dot(n, capsule_normal));
    glm_vec3_divs(left_op, right_op, left_op);
    float r = glm_vec3_dot(n, left_op);

    vec3 line_plane_intersection;
    glm_vec3_scale(capsule_normal, r, line_plane_intersection);
    glm_vec3_add(line_plane_intersection, base, line_plane_intersection);

    // Compute the cross products of the vector from the base of each edge
    // to the point with each edge vector.
    vec3 e0, e1, e2;
    glm_vec3_sub(t.p1, t.p0, e0);
    glm_vec3_sub(t.p2, t.p1, e1);
    glm_vec3_sub(t.p0, t.p2, e2);

    vec3 p0, p1, p2;
    glm_vec3_sub(line_plane_intersection, t.p0, p0);
    glm_vec3_sub(line_plane_intersection, t.p1, p1);
    glm_vec3_sub(line_plane_intersection, t.p2, p2);

    vec3 c0, c1, c2;
    glm_vec3_cross(p0, e0, c0);
    glm_vec3_cross(p1, e1, c1);
    glm_vec3_cross(p2, e2, c2);

    // If the cross product points in the same direction as the normal the
    // the point is inside the edge (it is zero if is on the edge).
    bool inside = glm_vec3_dot(c0, n) <= 0 && glm_vec3_dot(c1, n) <= 0 && glm_vec3_dot(c2, n) <= 0;

    if (inside) {
      glm_vec3_copy(line_plane_intersection, reference_point);
    } else {
      vec3 point1, point2, point3;
      // Edge 1:
      __closest_point_on_line_segment(t.p0, t.p1, line_plane_intersection, point1);

      // Edge 2:
      __closest_point_on_line_segment(t.p1, t.p2, line_plane_intersection, point2);

      // Edge 3:
      __closest_point_on_line_segment(t.p2, t.p0, line_plane_intersection, point3);

      glm_vec3_copy(point1, reference_point);

      vec3 best_dist_vec;
      glm_vec3_sub(point1, line_plane_intersection, best_dist_vec);
      float best_dist = glm_vec3_norm2(best_dist_vec);
      /* float best_dist = vec3_len_sq(vec3_sub(point1, line_plane_intersection)); */

      vec3 dist_vec;
      float dist;

      glm_vec3_sub(point2, line_plane_intersection, dist_vec);
      dist = fabsf(glm_vec3_norm2(dist_vec));

      /* float d = fabsf(vec3_len_sq(vec3_sub(point2, line_plane_intersection))); */
      if (dist < best_dist) {
        best_dist = dist;
        glm_vec3_copy(point2, reference_point);
      }
      glm_vec3_sub(point3, line_plane_intersection, dist_vec);
      dist = fabsf(glm_vec3_norm2(dist_vec));

      if (dist < best_dist) {
        best_dist = dist;
        glm_vec3_copy(point3, reference_point);
      }
    }
  }

  // Place a sphere on closest point on line segment to intersection:
  vec3 center;
  __closest_point_on_line_segment(a, b, reference_point, center);

  sphere_s sph;
  glm_vec3_copy(center, sph.center);
  sph.radius = radius;

  collision_check_sphere_triangle(sph, t, result);
}

// void collision_check_capsule_triangle(capsule_s c, triangle_s t, intersect_result_s *result) {
//
//   vec3 base = c.base;
//   vec3 tip = c.tip;
//   float radius = c.radius;
//   vec3 line_end_offset = vec3_scale(vec3_norm(vec3_sub(tip, base)), radius);
//   vec3 A = vec3_add(base, line_end_offset);
//   vec3 B = vec3_sub(tip, line_end_offset);
//
//   // Compute the plane of the triangle (has to be normalized).
//   vec3 N = vec3_norm(vec3_cross(vec3_sub(t.p1, t.p0), vec3_sub(t.p2, t.p0)));
//
//   // SM_ASSERT that the triangle is not degenerate.
//   SM_ASSERT(vec3_not_equal(N, vec3_zero()));
//
//   vec3 reference_point;
//   vec3 CapsuleNormal = vec3_norm(vec3_sub(B, A));
//   if (fabsf(vec3_dot(N, CapsuleNormal)) < SM_FLT_EPSILON) {
//     // Capsule line cannot be intersected with triangle plane (they are
//     // parallel)
//     //	In this case, just take a point from triangle
//     reference_point = t.p0;
//   } else {
//
//     // Intersect capsule line with triangle plane:
//     vec3 leftop = vec3_sub(base, t.p0);
//     float rightop = fabsf(vec3_dot(N, CapsuleNormal));
//     float r = vec3_dot(N, vec3_div_scalar(leftop, rightop));
//
//     vec3 line_plane_intersection = vec3_add(vec3_scale(CapsuleNormal, r), base);
//
//     // Compute the cross products of the vector from the base of each edge
//     // to the point with each edge vector.
//     vec3 c0 = vec3_cross(vec3_sub(line_plane_intersection, t.p0), vec3_sub(t.p1, t.p0));
//     vec3 c1 = vec3_cross(vec3_sub(line_plane_intersection, t.p1), vec3_sub(t.p2, t.p1));
//     vec3 c2 = vec3_cross(vec3_sub(line_plane_intersection, t.p2), vec3_sub(t.p0, t.p2));
//
//     // If the cross product points in the same direction as the normal the
//     // the point is inside the edge (it is zero if is on the edge).
//     bool inside = vec3_dot(c0, N) <= 0 && vec3_dot(c1, N) <= 0 && vec3_dot(c2, N) <= 0;
//
//     if (inside) {
//
//       reference_point = line_plane_intersection;
//     } else {
//
//       // Edge 1:
//       vec3 point1 = __closest_point_on_line_segment(t.p0, t.p1, line_plane_intersection);
//
//       // Edge 2:
//       vec3 point2 = __closest_point_on_line_segment(t.p1, t.p2, line_plane_intersection);
//
//       // Edge 3:
//       vec3 point3 = __closest_point_on_line_segment(t.p2, t.p0, line_plane_intersection);
//
//       reference_point = point1;
//       float best_dist = vec3_len_sq(vec3_sub(point1, line_plane_intersection));
//       float d = fabsf(vec3_len_sq(vec3_sub(point2, line_plane_intersection)));
//       if (d < best_dist) {
//         best_dist = d;
//         reference_point = point2;
//       }
//       d = fabsf(vec3_len_sq(vec3_sub(point3, line_plane_intersection)));
//       if (d < best_dist) {
//         best_dist = d;
//         reference_point = point3;
//       }
//     }
//   }
//
//   // Place a sphere on closest point on line segment to intersection:
//   vec3 center = __closest_point_on_line_segment(A, B, reference_point);
//
//   sphere_s sph = {.center = center, .radius = radius};
//
//   collision_check_sphere_triangle(sph, t, result);
// }

void collision_check_sphere_mesh(sphere_s s, mesh_s *mesh, intersect_result_s *result) {

  triangle_s t;

  bounding_box_s s_aabb = shapes_get_aabb_sphere(s);

  bounding_box_s mesh_aabb = mesh_get_bounding_box(mesh);
  if (!check_collision_bounding_box(s_aabb, mesh_aabb))
    return;

  for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->vertex.positions); i += 3) {

    glm_vec3_copy(mesh->vertex.positions[i + 0], t.p0);
    glm_vec3_copy(mesh->vertex.positions[i + 1], t.p1);
    glm_vec3_copy(mesh->vertex.positions[i + 2], t.p2);

    bounding_box_s t_aabb = shapes_get_aabb_triangle(t);

    if (!check_collision_bounding_box(s_aabb, t_aabb))
      continue;

    collision_check_sphere_triangle(s, t, result);

    if (result->valid) {
      return;
    }
  }
}

// void collision_check_sphere_mesh(sphere_s s, mesh_s *mesh, intersect_result_s *result) {
//   triangle_s t;
//
//   bounding_box_s s_aabb = shapes_get_aabb_sphere(s);
//
//   bounding_box_s mesh_aabb = mesh_get_bounding_box(mesh);
//   if (!check_collision_bounding_box(s_aabb, mesh_aabb))
//     return;
//
//   for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->vertex.positions); i += 3) {
//
//     t.p0 = mesh->vertex.positions[i + 0];
//     t.p1 = mesh->vertex.positions[i + 1];
//     t.p2 = mesh->vertex.positions[i + 2];
//
//     bounding_box_s t_aabb = shapes_get_aabb_triangle(t);
//
//     if (!check_collision_bounding_box(s_aabb, t_aabb))
//       continue;
//
//     collision_check_sphere_triangle(s, t, result);
//
//     if (result->valid) {
//       return;
//     }
//   }
// }
//

void collision_check_capsule_mesh(capsule_s c, mesh_s *mesh, intersect_result_s *result) {

  triangle_s t;

  bounding_box_s c_aabb = shapes_get_aabb_capsule(c);

  bounding_box_s mesh_aabb = mesh_get_bounding_box(mesh);
  if (!check_collision_bounding_box(c_aabb, mesh_aabb))
    return;

  for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->vertex.positions); i += 3) {

    glm_vec3_copy(mesh->vertex.positions[i + 0], t.p0);
    glm_vec3_copy(mesh->vertex.positions[i + 1], t.p1);
    glm_vec3_copy(mesh->vertex.positions[i + 2], t.p2);

    bounding_box_s t_aabb = shapes_get_aabb_triangle(t);

    if (!check_collision_bounding_box(c_aabb, t_aabb))
      continue;

    collision_check_capsule_triangle(c, t, result);

    if (result->valid) {
      return;
    }
  }
}

// void collision_check_capsule_mesh(capsule_s c, mesh_s *mesh, intersect_result_s *result) {
//
//   triangle_s t;
//
//   bounding_box_s c_aabb = shapes_get_aabb_capsule(c);
//
//   bounding_box_s mesh_aabb = mesh_get_bounding_box(mesh);
//   if (!check_collision_bounding_box(c_aabb, mesh_aabb))
//     return;
//
//   for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->vertex.positions); i += 3) {
//
//     t.p0 = mesh->vertex.positions[i + 0];
//     t.p1 = mesh->vertex.positions[i + 1];
//     t.p2 = mesh->vertex.positions[i + 2];
//
//     bounding_box_s t_aabb = shapes_get_aabb_triangle(t);
//
//     if (!check_collision_bounding_box(c_aabb, t_aabb))
//       continue;
//
//     collision_check_capsule_triangle(c, t, result);
//
//     if (result->valid) {
//       return;
//     }
//   }
// }

// collision_check_spheres - Check if two spheres are colliding.
void collision_check_spheres(sphere_s s1, sphere_s s2, intersect_result_s *result) {

  vec3 d;
  glm_vec3_sub(s1.center, s2.center, d);

  float dist = glm_vec3_norm(d);

  if (dist < s1.radius + s2.radius) {
    // calculate the depth of the collision
    // and the normal of the collision
    // (the normal is the direction of the collision)
    // (the depth is the distance from the center of the sphere to the collision)
    float depth = s1.radius + s2.radius - dist;
    glm_vec3_normalize(d);

    result->valid = true;
    result->depth = depth;
    glm_vec3_copy(d, result->normal);
  }
}

// void collision_check_spheres(sphere_s s1, sphere_s s2, intersect_result_s *result) {
//
//   vec3 d = vec3_sub(s1.center, s2.center);
//   float dist = vec3_len(d);
//
//   if (dist < s1.radius + s2.radius) {
//     // calculate the depth of the collision
//     // and the normal of the collision
//     // (the normal is the direction of the collision)
//     // (the depth is the distance from the center of the sphere to the collision)
//     float depth = s1.radius + s2.radius - dist;
//     vec3 normal = vec3_norm(d);
//
//     result->valid = true;
//     result->depth = depth;
//     result->normal = normal;
//   }
// }

// collision_check_sphere_cube - Check if a sphere and a cube are colliding.
// The cube is defined by its center and its size.
// The sphere is defined by its center and its radius.
// The result is stored in the intersect_result_s structure.
// The result is true if the two objects are colliding.
// The result is false if the two objects are not colliding.
// The depth and the normal of the collision are stored in the intersect_result_s structure.
// The depth is the distance from the center of the sphere to the collision.
// The normal is the direction of the collision.
// The depth is negative if the sphere is in the cube.
// The normal is pointing out of the cube if the sphere is in the cube.
void collision_check_sphere_cube(sphere_s s, cube_s c, intersect_result_s *result) {

  vec3 d;
  glm_vec3_sub(s.center, c.center, d);

  // check if the sphere is inside the cube
  if (fabsf(d[0]) <= c.size[0] / 2.0f && fabsf(d[1]) <= c.size[1] / 2.0f && fabsf(d[2]) <= c.size[2] / 2.0f) {
    // calculate the depth of the collision
    // and the normal of the collision
    // (the normal is the direction of the collision)
    // (the depth is the distance from the center of the sphere to the collision)
    float depth = c.size[0] / 2.0f - fabsf(d[0]);
    vec3 normal;
    normal[0] = d[0] > 0.0f ? -1.0f : 1.0f;
    normal[1] = 0.0f;
    normal[2] = 0.0f;

    result->valid = true;
    result->depth = depth;
    glm_vec3_copy(normal, result->normal);
  }
}

// void collision_check_sphere_cube(sphere_s s, cube_s c, intersect_result_s *result) {
//
//   vec3 d = vec3_sub(s.center, c.center);
//
//   // check if the sphere is inside the cube
//   if (fabsf(d.x) <= c.size.x / 2.0f && fabsf(d.y) <= c.size.y / 2.0f && fabsf(d.z) <= c.size.z / 2.0f) {
//     // calculate the depth of the collision
//     // and the normal of the collision
//     // (the normal is the direction of the collision)
//     // (the depth is the distance from the center of the sphere to the collision)
//     float depth = c.size.x / 2.0f - fabsf(d.x);
//     vec3 normal = vec3_new(d.x > 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
//
//     result->valid = true;
//     result->depth = depth;
//     result->normal = normal;
//   }
// }
