#ifndef SM_GEOMETRY_2D_H
#define SM_GEOMETRY_2D_H

#include "math/smMath.h"

/* -------------------------------------------------------------------------- */
/* Point */
/* -------------------------------------------------------------------------- */

typedef struct sm__point2D {

  union {

    struct {
      float x;
      float y;
    };

    vec2 data;
  };

} sm_point2D;
#define sm_point2D_new(X, Y) ((sm_point2D){.x = X, .y = Y})

/* -------------------------------------------------------------------------- */
/* Line */
/* -------------------------------------------------------------------------- */

typedef struct sm__line2D {

  sm_point2D start;
  sm_point2D end;

} sm_line2D;

#define sm_line2D_new(START, END)                                                                                      \
  ((sm_line2D){.start = sm_point2D_new(START.x, START.y), .end = sm_point2D_new(END.x, END.y)})

float line2D_length(sm_line2D line);
float line2D_length_squared(sm_line2D line);

/* -------------------------------------------------------------------------- */
/* Circle */
/* -------------------------------------------------------------------------- */

typedef struct sm__circle2D {

  sm_point2D position;
  float radius;

} sm_circle2D;

#define sm_circle2D_new(POSITION, RADIUS)                                                                              \
  (sm_circle2D) {                                                                                                      \
    .position = CENTER, .radius = RADIUS                                                                               \
  }

/* -------------------------------------------------------------------------- */
/* Rectangle */
/* -------------------------------------------------------------------------- */

typedef struct sm__rectangle2D {

  sm_point2D origin;
  vec2 size;

} sm_rectangle2D;

#define sm_rectangle2D_new(ORIGIN, SIZE)                                                                               \
  (sm_rectangle2D) {                                                                                                   \
    .origin = ORIGIN, .size = SIZE                                                                                     \
  }

void rectangle2D_get_min(sm_rectangle2D rectangle, vec2 out);
void rectangle2D_get_max(sm_rectangle2D rectangle, vec2 out);

/* -------------------------------------------------------------------------- */
/* Oriented rectangle */
/* -------------------------------------------------------------------------- */

typedef struct sm__oriented_rectangle2D {

  sm_point2D position;
  vec2 half_extents;
  float rotation;

} sm_oriented_rectangle2D;

#define sm_oriented_rectangle2D_new(POSITION, HALF_EXTENTS, ROTATION)                                                  \
  (sm_oriented_rectangle2D) {                                                                                          \
    .position = POSITION, .half_extents = HALF_EXTENTS, .rotation = ROTATION                                           \
  }

/* -------------------------------------------------------------------------- */
/* Interval */
/* -------------------------------------------------------------------------- */

typedef struct sm__interval2D {

  union {

    struct {
      float min;
      float max;
    };

    vec2 data;
  };

} sm_interval2D;

#define sm_interval2D_new(MIN, MAX) ((sm_interval2D){.min = MIN, .max = MAX})
sm_interval2D interval2D_from_rectangle(sm_rectangle2D rectangle, vec2 axis);
bool interval2D_overlap_on_axis(sm_rectangle2D rectangle1, sm_rectangle2D rectangle2, vec2 axis);

/* Point */
bool point2D_on_line(sm_point2D point, sm_line2D line);
bool point2D_in_circle(sm_point2D point, sm_circle2D circle);
bool point2D_in_rectangle(sm_point2D point, sm_rectangle2D rectangle);
bool point2D_in_oriented_rectangle(sm_point2D point, sm_oriented_rectangle2D oriented_rectangle);

/* Line */
bool line2D_intersects_circle(sm_line2D line, sm_circle2D circle);
bool line2D_intersects_rectangle(sm_line2D line, sm_rectangle2D rectangle);
bool line2D_intersects_oriented_rectangle(sm_line2D line, sm_oriented_rectangle2D oriented_rectangle);

/* Circle */
bool circle2D_intersects_circle(sm_circle2D circle1, sm_circle2D circle2);
bool circle2D_intersects_rectangle(sm_circle2D circle, sm_rectangle2D rectangle);
bool circle2D_intersects_oriented_rectangle(sm_circle2D circle, sm_oriented_rectangle2D oriented_rectangle);

/* Rectangle */
bool rectangle2D_intersects_rectangle(sm_rectangle2D rectangle1, sm_rectangle2D rectangle2);
bool rectangle2D_intersects_oriented_rectangle(sm_rectangle2D rectangle, sm_oriented_rectangle2D oriented_rectangle);
bool rectangle2D_intersects_rectangle_SAT(sm_rectangle2D rectangle1, sm_rectangle2D rectangle2);

/* Containing */

/* given a set of points, find the smallest circle that contains all of them */
sm_circle2D circle2D_containing_points(sm_point2D *points, int count);

/* given a set of points, find the smallest rectangle that contains all of them */
sm_rectangle2D rectangle2D_containing_points(sm_point2D *points, int count);

#endif /* SM_GEOMETRY_2D_H */
