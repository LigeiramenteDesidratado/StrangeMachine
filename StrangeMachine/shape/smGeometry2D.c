#include "shape/smGeometry2D.h"
#include "cglm/mat2.h"
#include "cglm/vec2.h"
#include "math/smMath.h"

float line2D_length(line2D line) {
  vec2 out;
  glm_vec2_sub(line.end, line.start, out);
  return glm_vec2_norm(out);
}

float line2D_length_squared(line2D line) {
  vec2 out;
  glm_vec2_sub(line.end, line.start, out);
  return glm_vec2_norm2(out);
}

void rectangle2D_get_min(rectangle2D rectangle, vec2 out) {
  vec2 p1, p2;
  glm_vec2_copy(rectangle.origin, p1);
  glm_vec2_add(rectangle.origin, rectangle.size, p2);

  out[0] = fminf(p1[0], p2[0]);
  out[1] = fminf(p1[1], p2[1]);
}

void rectangle2D_get_max(rectangle2D rectangle, vec2 out) {
  vec2 p1, p2;
  glm_vec2_copy(rectangle.origin, p1);
  glm_vec2_add(rectangle.origin, rectangle.size, p2);

  out[0] = fmaxf(p1[0], p2[0]);
  out[1] = fmaxf(p1[1], p2[1]);
}

rectangle2D rectangle2D_from_min_max(vec2 min, vec2 max) {
  rectangle2D out;
  glm_vec2_copy(min, out.origin);
  glm_vec2_sub(max, min, out.size);
  return out;
}

interval2D interval2D_from_rectangle(rectangle2D rectangle, vec2 axis) {
  interval2D out;
  vec2 min, max;

  rectangle2D_get_min(rectangle, min);
  rectangle2D_get_max(rectangle, max);

  /* use the min and max to build a set of vertices */
  vec2 verts[] = {
      {min[0], min[1]},
      {min[0], max[1]},
      {max[0], max[1]},
      {max[0], min[1]},
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

bool interval2D_overlap_on_axis(rectangle2D rectangle1, rectangle2D rectangle2, vec2 axis) {
  interval2D a = interval2D_from_rectangle(rectangle1, axis);
  interval2D b = interval2D_from_rectangle(rectangle2, axis);

  return ((b.min <= a.max) && (a.min <= b.max));
}

/* Point containment */
bool point2D_on_line(point2D point, line2D line) {
  /* find the slope of the line */
  float dx = line.end[0] - line.start[0];
  float dy = line.end[1] - line.start[1];

  float slope = dy / dx;

  /* find the y-intercept of the line */
  float y_intercept = line.start[1] - slope * line.start[0];

  /* check line equation */
  return CMP(point[1], slope * point[0] + y_intercept);
}

bool point2D_in_circle(point2D point, circle2D circle) {

  line2D line = line2D_new(point, circle.position);

  return line2D_length_squared(line) < circle.radius * circle.radius;
}

bool point2D_in_rectangle(point2D point, rectangle2D rectangle) {
  vec2 min, max;
  rectangle2D_get_min(rectangle, min);
  rectangle2D_get_max(rectangle, max);

  return min[0] <= point[0] && min[1] <= point[1] && point[0] <= max[0] && point[1] <= max[1];
}

bool point2D_in_oriented_rectangle(point2D point, oriented_rectangle2D oriented_rectangle) {
  vec2 rot_vec;
  glm_vec2_sub(point, oriented_rectangle.position, rot_vec);
  float theta = -glm_rad(oriented_rectangle.rotation);

  mat2 z_rotation_2x2 = {{cosf(theta), sinf(theta)}, {-sinf(theta), cosf(theta)}};

  vec2 rotated_point;
  glm_mat2_mulv(z_rotation_2x2, rot_vec, rotated_point);

  // TODO: implement this

  return false;
}

/* Line containment */
bool line2D_intersects_rectangle(line2D line, rectangle2D rectangle);
bool line2D_intersects_oriented_rectangle(line2D line, oriented_rectangle2D oriented_rectangle);

bool line2D_intersects_circle(line2D line, circle2D circle) {
  vec2 ab;
  glm_vec2_sub(line.end, line.start, ab);

  vec2 ac;
  glm_vec2_sub(circle.position, line.start, ac);

  float ab_length_squared = glm_vec2_norm2(ab);

  float ac_dot_ab = glm_vec2_dot(ac, ab);

  float t = ac_dot_ab / ab_length_squared;

  if (t < 0.0f || t > 1.0f) {
    return false;
  }

  point2D closest_point;
  glm_vec2_scale(ab, t, closest_point);
  glm_vec2_add(line.start, closest_point, closest_point);

  line2D circle_to_closest_point;
  glm_vec2_copy(circle.position, circle_to_closest_point.start);
  glm_vec2_copy(closest_point, circle_to_closest_point.end);

  return line2D_length_squared(circle_to_closest_point) < circle.radius * circle.radius;
}

bool line2D_intersects_rectangle(line2D line, rectangle2D rectangle) {
  if (point2D_in_rectangle(line.start, rectangle) || point2D_in_rectangle(line.end, rectangle)) {
    return true;
  }

  vec2 norm;
  glm_vec2_sub(line.end, line.start, norm);
  glm_vec2_normalize(norm);

  norm[0] = (norm[0] != 0.0f) ? 1.0f / norm[0] : 0.0f;
  norm[1] = (norm[1] != 0.0f) ? 1.0f / norm[1] : 0.0f;

  vec2 min, max;
  rectangle2D_get_min(rectangle, min);
  rectangle2D_get_max(rectangle, max);

  glm_vec2_sub(min, line.start, min);
  glm_vec2_sub(max, line.start, max);

  glm_vec2_mul(min, norm, min);
  glm_vec2_mul(max, norm, max);

  float min_t = fmaxf(fminf(min[0], max[0]), fminf(min[1], max[1]));
  float max_t = fminf(fmaxf(min[0], max[0]), fmaxf(min[1], max[1]));

  if (max_t < 0.0f || min_t > max_t) {
    return false;
  }

  float t = (min_t < 0.0f) ? max_t : min_t;
  return t > 0.0f && t * t < line2D_length_squared(line);
}

bool line2D_intersects_oriented_rectangle(line2D line, oriented_rectangle2D oriented_rectangle) {
  // TODO: implement this
  return false;
}

/* Circle containment */

bool circle2D_intersects_circle(circle2D circle1, circle2D circle2) {
  float distance_squared = glm_vec2_distance2(circle1.position, circle2.position);
  return distance_squared <= (circle1.radius + circle2.radius) * (circle1.radius + circle2.radius);
}

bool circle2D_intersects_rectangle(circle2D circle, rectangle2D rectangle) {
  vec3 min, max;
  rectangle2D_get_min(rectangle, min);
  rectangle2D_get_max(rectangle, max);

  /* find the closest point on the rectangle to the circle */
  point2D closest_point;
  glm_vec2_copy(circle.position, closest_point);

  if (closest_point[0] < min[0]) {
    closest_point[0] = min[0];
  } else if (closest_point[0] > max[0]) {
    closest_point[0] = max[0];
  }

  /* check if the closest point is inside the circle */
  line2D line = line2D_new(circle.position, closest_point);
  return line2D_length_squared(line) < circle.radius * circle.radius;
}

bool circle2D_intersects_oriented_rectangle(circle2D circle, oriented_rectangle2D oriented_rectangle) {
  /* create a line from the center of the circle to the center of the oriented rectangle */
  vec2 r;
  glm_vec2_sub(circle.position, oriented_rectangle.position, r);

  float theta = -glm_rad(oriented_rectangle.rotation);

  mat2 z_rotation_2x2 = {{cosf(theta), sinf(theta)}, {-sinf(theta), cosf(theta)}};
  // TODO: implement this
  (void)z_rotation_2x2;
  return false;
}

/* Rectangle containment */
bool rectangle2D_intersects_rectangle(rectangle2D rectangle1, rectangle2D rectangle2) {
  vec3 min1, max1;
  rectangle2D_get_min(rectangle1, min1);
  rectangle2D_get_max(rectangle1, max1);

  vec3 min2, max2;
  rectangle2D_get_min(rectangle2, min2);
  rectangle2D_get_max(rectangle2, max2);

  /* check for overlap on X and Y axes separately */
  bool x_overlap = (min2[0] <= max1[0]) && (min1[0] <= max2[0]);
  bool y_overlap = (min2[1] <= max1[1]) && (min1[1] <= max2[1]);

  return x_overlap && y_overlap;
}

bool rectangle2D_intersects_rectangle_SAT(rectangle2D rectangle1, rectangle2D rectangle2) {
  /* there are two potential axes for the separation between the two rectangles, the X and the Y axis */
  vec2 axis_to_check[2] = {{1.0f, 0.0f}, {0.0f, 1.0f}};

  for (int i = 0; i < 2; ++i) {
    if (!interval2D_overlap_on_axis(rectangle1, rectangle2, axis_to_check[i])) {
      return false;
    }
  }

  return true;
}

circle2D circle2D_containing_points(point2D *points, int count) {
  point2D center;
  for (int i = 0; i < count; ++i) {
    glm_vec2_add(center, points[i], center);
  }

  /* divide by the number of points */
  glm_vec2_scale(center, 1.0f / (float)count, center);

  circle2D circle;
  glm_vec2_copy(center, circle.position);
  circle.radius = 0.0f;

  for (int i = 1; i < count; ++i) {
    vec2 sq;
    glm_vec2_sub(center, points[0], sq);

    float distance = glm_vec2_norm2(sq);
    if (distance > circle.radius) {
      circle.radius = distance;
    }
  }

  circle.radius = sqrtf(circle.radius);
  return circle;
}

rectangle2D rectangle2D_containing_points(point2D *points, int count) {
  vec2 min, max;
  glm_vec2_copy(points[0], min);
  glm_vec2_copy(points[0], max);

  for (int i = 1; i < count; ++i) {
    min[0] = points[i][0] < min[0] ? points[i][0] : min[0];
    min[1] = points[i][1] < min[1] ? points[i][1] : min[1];

    max[0] = points[i][0] > max[0] ? points[i][0] : max[0];
    max[1] = points[i][1] > max[1] ? points[i][1] : max[1];
  }

  return rectangle2D_from_min_max(min, max);
}
