#ifndef SM_GEOMETRY_2D_H
#define SM_GEOMETRY_2D_H

#include "math/smMath.h"

/* -------------------------------------------------------------------------- */
/* Point */
/* -------------------------------------------------------------------------- */

typedef vec2 point2D;
#define point2D_new(X, Y) vec2_new(X, Y)

/* -------------------------------------------------------------------------- */
/* Line */
/* -------------------------------------------------------------------------- */

typedef struct {

  point2D start;
  point2D end;

} line2D;

#define line2D_new(START, END) ((line2D){.start = vec2_new2(START), .end = vec2_new2(END)})

float line2D_length(line2D line);
float line2D_length_squared(line2D line);

/* -------------------------------------------------------------------------- */
/* Circle */
/* -------------------------------------------------------------------------- */

typedef struct {

  point2D position;
  float radius;

} circle2D;

#define circle2D_new(POSITION, RADIUS)                                                                                 \
  (circle2D) {                                                                                                         \
    .position = CENTER, .radius = RADIUS                                                                               \
  }

/* -------------------------------------------------------------------------- */
/* Rectangle */
/* -------------------------------------------------------------------------- */

typedef struct {

  point2D origin;
  vec2 size;

} rectangle2D;

#define rectangle2D_new(ORIGIN, SIZE)                                                                                  \
  (rectangle2D) {                                                                                                      \
    .origin = ORIGIN, .size = SIZE                                                                                     \
  }

void rectangle2D_get_min(rectangle2D rectangle, vec2 out);
void rectangle2D_get_max(rectangle2D rectangle, vec2 out);

/* -------------------------------------------------------------------------- */
/* Oriented rectangle */
/* -------------------------------------------------------------------------- */

typedef struct {

  point2D position;
  vec2 half_extents;
  float rotation;

} oriented_rectangle2D;

#define oriented_rectangle2D_new(POSITION, HALF_EXTENTS, ROTATION)                                                     \
  (oriented_rectangle2D) {                                                                                             \
    .position = POSITION, .half_extents = HALF_EXTENTS, .rotation = ROTATION                                           \
  }

/* -------------------------------------------------------------------------- */
/* Interval */
/* -------------------------------------------------------------------------- */

typedef struct {
  float min;
  float max;

} interval2D;

#define interval2D_new(MIN, MAX) ((interval2D){.min = MIN, .max = MAX})
interval2D interval2D_from_rectangle(rectangle2D rectangle, vec2 axis);
bool interval2D_overlap_on_axis(rectangle2D rectangle1, rectangle2D rectangle2, vec2 axis);

/* Point */
bool point2D_on_line(point2D point, line2D line);
bool point2D_in_circle(point2D point, circle2D circle);
bool point2D_in_rectangle(point2D point, rectangle2D rectangle);
bool point2D_in_oriented_rectangle(point2D point, oriented_rectangle2D oriented_rectangle);

/* Line */
bool line2D_intersects_circle(line2D line, circle2D circle);
bool line2D_intersects_rectangle(line2D line, rectangle2D rectangle);
bool line2D_intersects_oriented_rectangle(line2D line, oriented_rectangle2D oriented_rectangle);

/* Circle */
bool circle2D_intersects_circle(circle2D circle1, circle2D circle2);
bool circle2D_intersects_rectangle(circle2D circle, rectangle2D rectangle);
bool circle2D_intersects_oriented_rectangle(circle2D circle, oriented_rectangle2D oriented_rectangle);

/* Rectangle */
bool rectangle2D_intersects_rectangle(rectangle2D rectangle1, rectangle2D rectangle2);
bool rectangle2D_intersects_oriented_rectangle(rectangle2D rectangle, oriented_rectangle2D oriented_rectangle);
bool rectangle2D_intersects_rectangle_SAT(rectangle2D rectangle1, rectangle2D rectangle2);

/* Containing */

/* given a set of points, find the smallest circle that contains all of them */
circle2D circle2D_containing_points(point2D *points, int count);

/* given a set of points, find the smallest rectangle that contains all of them */
rectangle2D rectangle2D_containing_points(point2D *points, int count);

#endif /* SM_GEOMETRY_2D_H */
