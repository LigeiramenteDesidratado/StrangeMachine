#include "shape/smGeometry2D.h"
#include "cglm/mat2.h"
#include "cglm/vec2.h"
#include "math/smMath.h"

float line2D_length(sm_line2D line) {
  sm_vec2 out;
  glm_vec2_sub(line.end.data, line.start.data, out.data);
  return glm_vec2_norm(out.data);
}

float line2D_length_squared(sm_line2D line) {
  sm_vec2 out;
  glm_vec2_sub(line.end.data, line.start.data, out.data);
  return glm_vec2_norm2(out.data);
}

void rectangle2D_get_min(sm_rectangle2D rectangle, vec2 out) {
  sm_vec2 p1, p2;
  glm_vec2_copy(rectangle.origin.data, p1.data);
  glm_vec2_add(rectangle.origin.data, rectangle.size, p2.data);

  out[0] = fminf(p1.x, p2.x);
  out[1] = fminf(p1.y, p2.y);
}

void rectangle2D_get_max(sm_rectangle2D rectangle, vec2 out) {
  sm_vec2 p1, p2;
  glm_vec2_copy(rectangle.origin.data, p1.data);
  glm_vec2_add(rectangle.origin.data, rectangle.size, p2.data);

  out[0] = fmaxf(p1.x, p2.x);
  out[1] = fmaxf(p1.y, p2.y);
}

sm_rectangle2D rectangle2D_from_min_max(vec2 min, vec2 max) {
  sm_rectangle2D out;
  glm_vec2_copy(min, out.origin.data);
  glm_vec2_sub(max, min, out.size);
  return out;
}

sm_interval2D interval2D_from_rectangle(sm_rectangle2D rectangle, vec2 axis) {
  sm_interval2D out;
  sm_vec2 min, max;

  rectangle2D_get_min(rectangle, min.data);
  rectangle2D_get_max(rectangle, max.data);

  /* use the min and max to build a set of vertices */
  vec2 verts[] = {
      {min.x, min.y},
      {min.x, max.y},
      {max.x, max.y},
      {max.x, min.y},
  };

  /* project each vertex onto the axis */
  out.min = glm_vec2_dot(axis, verts[0]);
  out.max = out.min;

  for (int i = 1; i < 4; ++i) {
    float projection = glm_vec2_dot(axis, verts[i]);
    if (projection < out.min) {
      out.min = projection;
    } else if (projection > out.max) {
      out.max = projection;
    }
  }

  return out;
}

bool interval2D_overlap_on_axis(sm_rectangle2D rectangle1, sm_rectangle2D rectangle2, vec2 axis) {
  sm_interval2D a = interval2D_from_rectangle(rectangle1, axis);
  sm_interval2D b = interval2D_from_rectangle(rectangle2, axis);

  return ((b.min <= a.max) && (a.min <= b.max));
}

/* Point containment */
bool point2D_on_line(sm_point2D point, sm_line2D line) {
  /* find the slope of the line */
  float dx = line.end.x - line.start.x;
  float dy = line.end.y - line.start.y;

  float slope = dy / dx;

  /* find the y-intercept of the line */
  float y_intercept = line.start.y - slope * line.start.x;

  /* check line equation */
  return CMP(point.y, slope * point.x + y_intercept);
}

bool point2D_in_circle(sm_point2D point, sm_circle2D circle) {

  sm_line2D line = sm_line2D_new(point, circle.position);

  return line2D_length_squared(line) < circle.radius * circle.radius;
}

bool point2D_in_rectangle(sm_point2D point, sm_rectangle2D rectangle) {
  sm_vec2 min, max;

  rectangle2D_get_min(rectangle, min.data);
  rectangle2D_get_max(rectangle, max.data);

  return min.x <= point.x && min.y <= point.y && point.x <= max.x && point.y <= max.y;
}

bool point2D_in_oriented_rectangle(sm_point2D point, sm_oriented_rectangle2D oriented_rectangle) {
  vec2 rot_vec;
  glm_vec2_sub(point.data, oriented_rectangle.position.data, rot_vec);
  float theta = -glm_rad(oriented_rectangle.rotation);

  mat2 z_rotation_2x2 = {{cosf(theta), sinf(theta)}, {-sinf(theta), cosf(theta)}};

  vec2 rotated_point;
  glm_mat2_mulv(z_rotation_2x2, rot_vec, rotated_point);

  // TODO: implement this

  return false;
}

/* Line containment */
bool line2D_intersects_rectangle(sm_line2D line, sm_rectangle2D rectangle);
bool line2D_intersects_oriented_rectangle(sm_line2D line, sm_oriented_rectangle2D oriented_rectangle);

bool line2D_intersects_circle(sm_line2D line, sm_circle2D circle) {
  vec2 ab;
  glm_vec2_sub(line.end.data, line.start.data, ab);

  vec2 ac;
  glm_vec2_sub(circle.position.data, line.start.data, ac);

  float ab_length_squared = glm_vec2_norm2(ab);

  float ac_dot_ab = glm_vec2_dot(ac, ab);

  float t = ac_dot_ab / ab_length_squared;

  if (t < 0.0f || t > 1.0f) {
    return false;
  }

  sm_point2D closest_point;
  glm_vec2_scale(ab, t, closest_point.data);
  glm_vec2_add(line.start.data, closest_point.data, closest_point.data);

  sm_line2D circle_to_closest_point;
  glm_vec2_copy(circle.position.data, circle_to_closest_point.start.data);
  glm_vec2_copy(closest_point.data, circle_to_closest_point.end.data);

  return line2D_length_squared(circle_to_closest_point) < circle.radius * circle.radius;
}

bool line2D_intersects_rectangle(sm_line2D line, sm_rectangle2D rectangle) {
  if (point2D_in_rectangle(line.start, rectangle) || point2D_in_rectangle(line.end, rectangle)) {
    return true;
  }

  sm_vec2 norm;
  glm_vec2_sub(line.end.data, line.start.data, norm.data);
  glm_vec2_normalize(norm.data);

  norm.x = (norm.x != 0.0f) ? 1.0f / norm.x : 0.0f;
  norm.y = (norm.y != 0.0f) ? 1.0f / norm.y : 0.0f;

  vec2 min, max;
  rectangle2D_get_min(rectangle, min);
  rectangle2D_get_max(rectangle, max);

  glm_vec2_sub(min, line.start.data, min);
  glm_vec2_sub(max, line.start.data, max);

  glm_vec2_mul(min, norm.data, min);
  glm_vec2_mul(max, norm.data, max);

  float min_t = fmaxf(fminf(min[0], max[0]), fminf(min[1], max[1]));
  float max_t = fminf(fmaxf(min[0], max[0]), fmaxf(min[1], max[1]));

  if (max_t < 0.0f || min_t > max_t) {
    return false;
  }

  float t = (min_t < 0.0f) ? max_t : min_t;
  return t > 0.0f && t * t < line2D_length_squared(line);
}

bool line2D_intersects_oriented_rectangle(sm_line2D line, sm_oriented_rectangle2D oriented_rectangle) {
  // TODO: implement this
  return false;
}

/* Circle containment */

bool circle2D_intersects_circle(sm_circle2D circle1, sm_circle2D circle2) {
  float distance_squared = glm_vec2_distance2(circle1.position.data, circle2.position.data);
  return distance_squared <= (circle1.radius + circle2.radius) * (circle1.radius + circle2.radius);
}

bool circle2D_intersects_rectangle(sm_circle2D circle, sm_rectangle2D rectangle) {
  sm_vec3 min, max;
  rectangle2D_get_min(rectangle, min.data);
  rectangle2D_get_max(rectangle, max.data);

  /* find the closest point on the rectangle to the circle */
  sm_point2D closest_point;
  glm_vec2_copy(circle.position.data, closest_point.data);

  if (closest_point.x < min.x) {
    closest_point.x = min.x;
  } else if (closest_point.x > max.x) {
    closest_point.x = max.x;
  }

  /* check if the closest point is inside the circle */
  sm_line2D line = sm_line2D_new(circle.position, closest_point);
  return line2D_length_squared(line) < circle.radius * circle.radius;
}

bool circle2D_intersects_oriented_rectangle(sm_circle2D circle, sm_oriented_rectangle2D oriented_rectangle) {
  /* create a line from the center of the circle to the center of the oriented rectangle */
  sm_vec2 r;
  glm_vec2_sub(circle.position.data, oriented_rectangle.position.data, r.data);

  float theta = -glm_rad(oriented_rectangle.rotation);

  mat2 z_rotation_2x2 = {{cosf(theta), sinf(theta)}, {-sinf(theta), cosf(theta)}};
  // TODO: implement this
  (void)z_rotation_2x2;
  return false;
}

/* Rectangle containment */
bool rectangle2D_intersects_rectangle(sm_rectangle2D rectangle1, sm_rectangle2D rectangle2) {
  sm_vec3 min1, max1;
  rectangle2D_get_min(rectangle1, min1.data);
  rectangle2D_get_max(rectangle1, max1.data);

  sm_vec3 min2, max2;
  rectangle2D_get_min(rectangle2, min2.data);
  rectangle2D_get_max(rectangle2, max2.data);

  /* check for overlap on X and Y axes separately */
  bool x_overlap = (min2.x <= max1.x) && (min1.x <= max2.x);
  bool y_overlap = (min2.y <= max1.y) && (min1.y <= max2.y);

  return x_overlap && y_overlap;
}

bool rectangle2D_intersects_rectangle_SAT(sm_rectangle2D rectangle1, sm_rectangle2D rectangle2) {
  /* there are two potential axes for the separation between the two rectangles, the X and the Y axis */
  sm_vec2 axis_to_check[2] = {sm_vec2_new(1.0f, 0.0f), sm_vec2_new(0.0f, 1.0f)};

  for (int i = 0; i < 2; ++i) {
    if (!interval2D_overlap_on_axis(rectangle1, rectangle2, axis_to_check[i].data)) {
      return false;
    }
  }

  return true;
}

sm_circle2D circle2D_containing_points(sm_point2D *points, int count) {
  sm_point2D center;
  for (int i = 0; i < count; ++i) {
    glm_vec2_add(center.data, points[i].data, center.data);
  }

  /* divide by the number of points */
  glm_vec2_scale(center.data, 1.0f / (float)count, center.data);

  sm_circle2D circle;
  glm_vec2_copy(center.data, circle.position.data);
  circle.radius = 0.0f;

  for (int i = 1; i < count; ++i) {
    sm_vec2 sq;
    glm_vec2_sub(center.data, points[0].data, sq.data);

    float distance = glm_vec2_norm2(sq.data);
    if (distance > circle.radius) {
      circle.radius = distance;
    }
  }

  circle.radius = sqrtf(circle.radius);
  return circle;
}

sm_rectangle2D rectangle2D_containing_points(sm_point2D *points, int count) {
  sm_vec2 min, max;
  glm_vec2_copy(points[0].data, min.data);
  glm_vec2_copy(points[0].data, max.data);

  for (int i = 1; i < count; ++i) {
    min.x = points[i].x < min.x ? points[i].x : min.x;
    min.y = points[i].y < min.y ? points[i].y : min.y;

    max.x = points[i].x > max.x ? points[i].x : max.x;
    max.y = points[i].y > max.y ? points[i].y : max.y;
  }

  return rectangle2D_from_min_max(min.data, max.data);
}
