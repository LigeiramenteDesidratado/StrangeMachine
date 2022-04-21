

#include "smGeometry3D.h"

float line3D_length(line3D line) {
  vec3 distance;
  glm_vec3_sub(line.start, line.end, distance);

  return glm_vec3_norm(distance);
}

float line3D_length_squared(line3D line) {
  vec3 distance;
  glm_vec3_sub(line.start, line.end, distance);

  return glm_vec3_norm2(distance);
}

ray3D ray3D_from_points(point3D from, point3D to) {
  vec3 normalized_direction;
  glm_vec3_sub(to, from, normalized_direction);
  glm_vec3_normalize(normalized_direction);

  ray3D ray;
  glm_vec3_copy(normalized_direction, ray.direction);
  glm_vec3_copy(from, ray.origin);

  return ray;
}

void AABB3D_get_min(AABB3D aabb, vec3 out) {
  vec3 p1, p2;
  glm_vec3_add(aabb.position, aabb.size, p1);
  glm_vec3_sub(aabb.position, aabb.size, p2);

  glm_vec3_copy(vec3_new(fminf(p1[0], p2[0]), fminf(p1[1], p2[1]), fminf(p1[2], p2[2])), out);
}

void AABB3D_get_max(AABB3D aabb, vec3 out) {
  vec3 p1, p2;
  glm_vec3_add(aabb.position, aabb.size, p1);
  glm_vec3_sub(aabb.position, aabb.size, p2);

  glm_vec3_copy(vec3_new(fmaxf(p1[0], p2[0]), fmaxf(p1[1], p2[1]), fmaxf(p1[2], p2[2])), out);
}

AABB3D AABB3D_from_min_max(vec3 min, vec3 max) {
  AABB3D aabb;

  point3D position;
  glm_vec3_add(min, max, position);
  glm_vec3_scale(position, 0.5f, position);
  glm_vec3_copy(position, aabb.position);

  vec3 size;
  glm_vec3_sub(max, min, size);
  glm_vec3_scale(size, 0.5f, size);
  glm_vec3_copy(size, aabb.size);

  return aabb;
}

float plane3D_equation(plane3D plane, point3D point) {
  return glm_vec3_dot(point, plane.normal) - plane.distance;
}

bool point3D_in_sphere(point3D point, sphere3D sphere) {
  vec3 distance;
  glm_vec3_sub(point, sphere.position, distance);

  float distance_squared = glm_vec3_norm2(distance);
  float radius_squared = sphere.radius * sphere.radius;

  return distance_squared < radius_squared;
}

void point3D_closest_point_on_sphere(point3D point, sphere3D sphere, point3D out) {

  vec3 sphere_to_point;
  glm_vec3_sub(point, sphere.position, sphere_to_point);
  glm_vec3_normalize(sphere_to_point);

  /* resize the normalized vector to the sphere radius */
  glm_vec3_scale(sphere_to_point, sphere.radius, sphere_to_point);

  /* return the resized vector offset by the sphere position */
  glm_vec3_add(sphere_to_point, sphere.position, out);
}

bool point3D_in_AABB(point3D point, AABB3D aabb) {
  vec3 min, max;
  AABB3D_get_min(aabb, min);
  AABB3D_get_max(aabb, max);

  if (point[0] < min[0] || point[1] < min[1] || point[2] < min[2]) {
    return false;
  }

  if (point[0] > max[0] || point[1] > max[1] || point[2] > max[2]) {
    return false;
  }

  return true;
}

void point3D_closest_point_on_AABB(point3D point, AABB3D aabb, point3D out) {
  vec3 min, max;
  AABB3D_get_min(aabb, min);
  AABB3D_get_max(aabb, max);

  point3D result;
  glm_vec3_copy(point, result);

  result[0] = (result[0] < min[0]) ? min[0] : result[0];
  result[1] = (result[1] < min[1]) ? min[1] : result[1];
  result[2] = (result[2] < min[2]) ? min[2] : result[2];

  result[0] = (result[0] > max[0]) ? max[0] : result[0];
  result[1] = (result[1] > max[1]) ? max[1] : result[1];
  result[2] = (result[2] > max[2]) ? max[2] : result[2];

  glm_vec3_copy(result, out);
}

bool point3D_in_OBB(point3D point, OBB3D obb) {
  vec3 dir;
  glm_vec3_sub(point, obb.position, dir);

  for (int i = 0; i < 3; ++i) {

    vec3 axis;
    axis[0] = obb.orientation[i][0];
    axis[1] = obb.orientation[i][1];
    axis[2] = obb.orientation[i][2];

    float dot = glm_vec3_dot(dir, axis);

    if (dot > obb.size[i])
      return false;
    if (dot < -obb.size[i])
      return false;
  }

  return true;
}

void point3D_closest_point_on_OBB(point3D point, OBB3D obb, point3D out) {
  vec3 dir;
  glm_vec3_sub(point, obb.position, dir);

  point3D result;
  glm_vec3_copy(obb.position, result);

  for (int i = 0; i < 3; ++i) {

    vec3 axis;
    axis[0] = obb.orientation[i][0];
    axis[1] = obb.orientation[i][1];
    axis[2] = obb.orientation[i][2];

    float dot = glm_vec3_dot(dir, axis);

    if (dot > obb.size[i]) {
      dot = obb.size[i];
    }
    if (dot < -obb.size[i]) {
      dot = -obb.size[i];
    }

    vec3 axis_scaled;
    glm_vec3_scale(axis, dot, axis_scaled);
    glm_vec3_add(result, axis_scaled, result);
  }

  glm_vec3_copy(result, out);
}

bool point3D_in_plane(point3D point, plane3D plane) {
  float dot = glm_vec3_dot(point, plane.normal);

  return dot - plane.distance == 0.0f;
}

void point3D_closest_point_on_plane(point3D point, plane3D plane, point3D out) {
  float dot = glm_vec3_dot(plane.normal, point);
  float distance = dot - plane.distance;

  vec3 point_on_plane;
  glm_vec3_scale(plane.normal, distance, point_on_plane);
  glm_vec3_sub(point, point_on_plane, out);
}

bool point3D_in_line(point3D point, line3D line) {
  point3D closest_point;
  point3D_closest_point_on_line(point, line, closest_point);

  vec3 distance;
  glm_vec3_sub(closest_point, point, distance);
  float distance_squared = glm_vec3_norm2(distance);

  return distance_squared < 0.0001f;
}

void point3D_closest_point_on_line(point3D point, line3D line, point3D out) {

  vec3 line_dir;
  glm_vec3_sub(line.end, line.start, line_dir);

  vec3 point_to_line_start;
  glm_vec3_sub(point, line.start, point_to_line_start);

  float t = glm_vec3_dot(point_to_line_start, line_dir) / glm_vec3_dot(line_dir, line_dir);

  t = fmax(t, 0.0f);
  t = fmin(t, 1.0f);

  vec3 result;
  glm_vec3_scale(line_dir, t, result);
  glm_vec3_add(result, line.start, out);
}

bool point3D_in_ray(point3D point, ray3D ray) {
  if (glm_vec3_eqv_eps(ray.origin, point)) {
    return true;
  }

  vec3 norm;
  glm_vec3_sub(point, ray.origin, norm);

  float diff = glm_vec3_dot(norm, ray.direction);

  /* return diff == 1.0f; */
  return CMP(diff, 1.0f);
}

void point3D_closest_point_on_ray(point3D point, ray3D ray, point3D out) {
  vec3 norm;
  glm_vec3_sub(point, ray.origin, norm);

  float t = glm_vec3_dot(norm, ray.direction);

  t = fmax(t, 0.0f);

  vec3 result;
  glm_vec3_scale(ray.direction, t, result);
  glm_vec3_add(ray.origin, result, out);
}
