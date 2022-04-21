#ifndef SM_GEOMETRY_3D_H
#define SM_GEOMETRY_3D_H

#include "math/smMath.h"

/* -------------------------------------------------------------------------- */
/* point3D */
/* -------------------------------------------------------------------------- */

typedef vec3 point3D;
#define point3D_new(X, Y, Z) vec3_new(X, Y, Z)

/* -------------------------------------------------------------------------- */
/* line3D */
/* -------------------------------------------------------------------------- */
typedef struct {

  point3D start;
  point3D end;

} line3D;

#define line3D_new(START, END)                                                                                         \
  { START, END }
float line3D_length(line3D line);
float line3D_length_squared(line3D line);

/* -------------------------------------------------------------------------- */
/* ray3D */
/* -------------------------------------------------------------------------- */
typedef struct {

  point3D origin;
  vec3 direction;

} ray3D;

#define ray3D_new(ORIGIN, DIRECTION) ((ray3D){ORIGIN, DIRECTION})
ray3D ray3D_from_points(point3D from, point3D to);

/* -------------------------------------------------------------------------- */
/* sphere3D */
/* -------------------------------------------------------------------------- */
typedef struct {

  point3D position;
  float radius;

} sphere3D;

#define sphere3D_new(POSITION, RADIUS) ((sphere3D){POSITION, RADIUS})

/* -------------------------------------------------------------------------- */
/* AABB3D */
/* -------------------------------------------------------------------------- */
typedef struct {

  point3D position;
  vec3 size;

} AABB3D;

#define AABB3D_new(ORIGIN, SIZE) ((AABB3D){ORIGIN, SIZE})
void AABB3D_get_min(AABB3D aabb, vec3 out);
void AABB3D_get_max(AABB3D aabb, vec3 out);
AABB3D AABB3D_from_min_max(vec3 min, vec3 max);

/* -------------------------------------------------------------------------- */
/* OBB3D */
/* -------------------------------------------------------------------------- */
typedef struct {

  point3D position;
  vec3 size;
  mat3 orientation;

} OBB3D;

#define OBB3D_new(POSITION, SIZE, ORIENTATION) ((OBB3D){POSITION, SIZE, ORIENTATION})

/* -------------------------------------------------------------------------- */
/* plane3D */
/* -------------------------------------------------------------------------- */
typedef struct {

  vec3 normal;
  float distance;

} plane3D;

#define plane3D_new(NORMAL, DISTANCE) ((plane3D){NORMAL, DISTANCE})
float plane3D_equation(plane3D plane, point3D point);

/* -------------------------------------------------------------------------- */
/* triangle3D */
/* -------------------------------------------------------------------------- */
typedef union {

  struct {
    point3D a;
    point3D b;
    point3D c;
  };
  struct {
    point3D p1;
    point3D p2;
    point3D p3;
  };

  point3D points[3];

  float values[3];

} triangle3D;

#define triangle3D_new(A, B, C) ((triangle3D){A, B, C})

/* point and sphere */
bool point3D_in_sphere(point3D point, sphere3D sphere);
void point3D_closest_point_on_sphere(point3D point, sphere3D sphere, point3D out);

/* point and AABB */
bool point3D_in_AABB(point3D point, AABB3D aabb);
void point3D_closest_point_on_AABB(point3D point, AABB3D aabb, point3D out);

/* point and OBB */
bool point3D_in_OBB(point3D point, OBB3D obb);
void point3D_closest_point_on_OBB(point3D point, OBB3D obb, point3D out);

/* point and plane */
bool point3D_in_plane(point3D point, plane3D plane);
void point3D_closest_point_on_plane(point3D point, plane3D plane, point3D out);

/* point and line */
bool point3D_in_line(point3D point, line3D line);
void point3D_closest_point_on_line(point3D point, line3D line, point3D out);

/* point and ray */
bool point3D_in_ray(point3D point, ray3D ray);
void point3D_closest_point_on_ray(point3D point, ray3D ray, point3D out);

#endif /* SM_GEOMETRY_3D_H */
