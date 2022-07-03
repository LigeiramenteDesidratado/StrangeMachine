#ifndef SM_MATH_MATH_H
#define SM_MATH_MATH_H

#include "smpch.h"

#include "cglm/cglm.h"

#define CMP(X, Y) (fabsf((X) - (Y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(X), fabsf(Y))))
#define EPSILON   0.000001f

/* Use the same memory for the anonynous structure and the byte array field `data` */
typedef struct sm__vec2 {

  union {

    struct {
      f32 x, y;
    };

    vec2 data;
  };

} sm_vec2;

#define sm_vec2_print(V)  printf("%s\n\t%f, %f\n", #V, V.x, V.y);
#define sm_vec2_new(X, Y) ((sm_vec2){.x = X, .y = Y})
#define sm_vec2_zero()    ((sm_vec2){.x = 0.0f, .y = 0.0f})
#define sm_vec2_one()     ((sm_vec2){.x = 1.0f, .y = 1.0f})

/* A vector with 2 int32_t components */
typedef struct sm__ivec2 {
  union {

    struct {
      i32 x;
      i32 y;
    };

    i32 v[2];
  };

} sm_ivec2;

#define sm_ivec2_print(V)  printf("%s\n\t%d, %d\n", #V, V.x, V.y);
#define sm_ivec2_new(X, Y) ((sm_ivec2){.x = X, .y = Y})
#define sm_ivec2_zero()    ((sm_ivec2){.x = 0, .y = 0})
#define sm_ivec2_one()     ((sm_ivec2){.x = 1, .y = 1})

/* A vector with 2 u32 components */
typedef struct sm__uivec2 {

  union {
    struct {
      u32 x;
      u32 y;
    };

    u32 data[2];
  };

} sm_uivec2;

#define sm_uivec2_print(V)  printf("%s\n\t%d, %d\n", #V, V.x, V.y);
#define sm_uivec2_new(X, Y) ((sm_uivec2){.x = X, .y = Y})
#define sm_uivec2_zero()    ((sm_uivec2){.x = 0, .y = 0})
#define sm_uivec2_one()     ((sm_uivec2){.x = 1, .y = 1})

/* Use the same memory for the anonymous structure and the byte array field `data` */

/* A vector with 3 f32 components */
typedef struct sm__vec3 {

  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
    };

    struct {
      f32 r;
      f32 g;
      f32 b;
    };

    struct {
      f32 pitch;
      f32 yaw;
      f32 roll;
    };

    vec3 data;
  };

} sm_vec3;

#define sm_vec3_print(V)     printf("%s\n\t%f, %f, %f\n", #V, V.x, V.y, V.z);
#define sm_vec3_new(X, Y, Z) ((sm_vec3){.x = X, .y = Y, .z = Z})
#define sm_vec3_zero()       ((sm_vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f})
#define sm_vec3_one()        ((sm_vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f})
#define sm_vec3_right()      sm_vec3_new(1.0f, 0.0f, 0.0f)
#define sm_vec3_up()         sm_vec3_new(0.0f, 1.0f, 0.0f)
#define sm_vec3_forward()    sm_vec3_new(0.0f, 0.0f, 1.0f)

/* A vector with 4 f32 components */
typedef CGLM_ALIGN_IF(16) struct sm__vec4 {

  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };

    struct {
      f32 r;
      f32 g;
      f32 b;
      f32 a;
    };

    vec4 data;
  };

} sm_vec4;

#define sm_vec4_print(V)        printf("%s\n\t%f, %f, %f, %f\n", #V, V.x, V.y, V.z, V.w);
#define sm_vec4_new(X, Y, Z, W) ((sm_vec4){.x = X, .y = Y, .z = Z, .w = W})
#define sm_vec4_zero()          ((sm_vec4){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f})
#define sm_vec4_one()           ((sm_vec4){.x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f})

/* A vector with 4 int32_t components */
typedef struct sm__ivec4 {

  union {
    struct {
      i32 x;
      i32 y;
      i32 z;
      i32 w;
    };

    i32 data[4];
  };

} sm_ivec4;

#define sm_ivec4_print(V)        printf("%s\n\t%d, %d, %d, %d\n", #V, V.x, V.y, V.z, V.w);
#define sm_ivec4_new(X, Y, Z, W) ((sm_ivec4){.x = X, .y = Y, .z = Z, .w = W})
#define sm_ivec4_zero()          ((sm_ivec4){.x = 0, .y = 0, .z = 0, .w = 0})
#define sm_ivec4_one()           ((sm_ivec4){.x = 1, .y = 1, .z = 1, .w = 1})

/* A vector with 4 u32 components */
typedef struct sm__uivec4 {

  union {
    struct {
      u32 x;
      u32 y;
      u32 z;
      u32 w;
    };

    u32 data[4];
  };

} sm_uivec4;

#define sm_uivec4_print(V)        printf("%s\n\t%d, %d, %d, %d\n", #V, V.x, V.y, V.z, V.w);
#define sm_uivec4_new(X, Y, Z, W) ((sm_uivec4){.x = X, .y = Y, .z = Z, .w = W})
#define sm_uivec4_zero()          ((sm_uivec4){.x = 0, .y = 0, .z = 0, .w = 0})
#define sm_uivec4_one()           ((sm_uivec4){.x = 1, .y = 1, .z = 1, .w = 1})

/* A matrix with 4x4 f32 components */
typedef CGLM_ALIGN_MAT struct sm__mat4 {

  union {

    struct {
      sm_vec4 right;    /* first column */
      sm_vec4 up;       /* second column */
      sm_vec4 forward;  /* third column */
      sm_vec4 position; /* fourth column */
    } vec4;

    struct {
      sm_vec3 right;       /* first column */
      f32 padding_rigth;   /* w value of right */
      sm_vec3 up;          /* second column */
      f32 padding_up;      /* w value of up */
      sm_vec3 forward;     /* third column */
      f32 padding_forward; /* w value of forward */
      sm_vec3 position;    /* fourth column */
      f32 w_position;

    } vec3;

    /* basis vector notation */
    struct {
      /* col 1 */ f32 xx, xy, xz, xw; /* right */
      /* col 2 */ f32 yx, yy, yz, yw; /* up */
      /* col 3 */ f32 zx, zy, zz, zw; /* forward */
      /* col 4 */ f32 tx, ty, tz, tw; /* position */
    };

    /* column-row notation */
    struct {
      f32 c0r0, c0r1, c0r2, c0r3;
      f32 c1r0, c1r1, c1r2, c1r3;
      f32 c2r0, c2r1, c2r2, c2r3;
      f32 c3r0, c3r1, c3r2, c3r3;
    };

    /* row-column notation */
    struct {
      f32 r0c0, r1c0, r2c0, r3c0;
      f32 r0c1, r1c1, r2c1, r3c1;
      f32 r0c2, r1c2, r2c2, r3c2;
      f32 r0c3, r1c3, r2c3, r3c3;
    };

    mat4 data;
    f32 float16[16];
  };

} sm_mat4;

#define sm_mat4_print(M)                                                                                               \
  printf("\t%s---------\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%"                                                            \
         "7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7."                                          \
         "3f|%7.3f|%7.3f|\n\t---------------------------------\n",                                                     \
         #M, M.float16[0], M.float16[1], M.float16[2], M.float16[3], M.float16[4], M.float16[5], M.float16[6],         \
         M.float16[7], M.float16[8], M.float16[9], M.float16[10], M.float16[11], M.float16[12], M.float16[13],         \
         M.float16[14], M.float16[15])

#define sm_mat4_new(inXX, inXY, inXZ, inXW, inYX, inYY, inYZ, inYW, inZX, inZY, inZZ, inZW, inTX, inTY, inTZ, inTW)    \
  ((sm_mat4){.xx = inXX,                                                                                               \
             .xy = inXY,                                                                                               \
             .xz = inXZ,                                                                                               \
             .xw = inXW,                                                                                               \
             .yx = inYX,                                                                                               \
             .yy = inYY,                                                                                               \
             .yz = inYZ,                                                                                               \
             .yw = inYW,                                                                                               \
             .zx = inZX,                                                                                               \
             .zy = inZY,                                                                                               \
             .zz = inZZ,                                                                                               \
             .zw = inZW,                                                                                               \
             .tx = inTX,                                                                                               \
             .ty = inTY,                                                                                               \
             .tz = inTZ,                                                                                               \
             .tw = inTW})

#define sm_mat4_zero()                                                                                                 \
  ((sm_mat4){.xx = 0.0f,                                                                                               \
             .xy = 0.0f,                                                                                               \
             .xz = 0.0f,                                                                                               \
             .xw = 0.0f,                                                                                               \
             .yx = 0.0f,                                                                                               \
             .yy = 0.0f,                                                                                               \
             .yz = 0.0f,                                                                                               \
             .yw = 0.0f,                                                                                               \
             .zx = 0.0f,                                                                                               \
             .zy = 0.0f,                                                                                               \
             .zz = 0.0f,                                                                                               \
             .zw = 0.0f,                                                                                               \
             .tx = 0.0f,                                                                                               \
             .ty = 0.0f,                                                                                               \
             .tz = 0.0f,                                                                                               \
             .tw = 0.0f})

#define sm_mat4_identity()                                                                                             \
  ((sm_mat4){.xx = 1.0f,                                                                                               \
             .xy = 0.0f,                                                                                               \
             .xz = 0.0f,                                                                                               \
             .xw = 0.0f,                                                                                               \
             .yx = 0.0f,                                                                                               \
             .yy = 1.0f,                                                                                               \
             .yz = 0.0f,                                                                                               \
             .yw = 0.0f,                                                                                               \
             .zx = 0.0f,                                                                                               \
             .zy = 0.0f,                                                                                               \
             .zz = 1.0f,                                                                                               \
             .zw = 0.0f,                                                                                               \
             .tx = 0.0f,                                                                                               \
             .ty = 0.0f,                                                                                               \
             .tz = 0.0f,                                                                                               \
             .tw = 1.0f})

typedef struct sm__transform_s {

  union {
    struct {
      sm_vec4 position; /* position x y z plus padding */
      sm_vec4 rotation; /* rotation x y z w*/
      sm_vec4 scale;    /* scale x y z plus padding */
    };

    vec4 data[3];
  };

} sm_transform_s;

#define sm_transform_new(POSITION, ROTATION, SCALE)                                                                    \
  ((sm_transform_s){.position = POSITION, .rotation = ROTATION, .scale = SCALE})

#define sm_transform_identity() ((sm_transform_s){sm_vec4_zero(), sm_vec4_new(0.0f, 0.0f, 0.0f, 1.0f), sm_vec4_one()})

#define sm_transform_print(T)                                                                                          \
  printf("%s:\n", #T);                                                                                                 \
  printf("posit: %f, %f, %f\n", (T).position.x, (T).position.y, (T).position.z);                                       \
  printf("rotat: %f, %f, %f, %f\n", (T).rotation.x, (T).rotation.y, (T).rotation.z, (T).rotation.w);                   \
  printf("scale: %f, %f, %f\n", (T).scale.x, (T).scale.y, (T).scale.z);

sm_transform_s transform_combine(sm_transform_s a, sm_transform_s b);
void transform_to_mat4(sm_transform_s t, mat4 out);
sm_transform_s transform_mat4_to_transform(sm_mat4 m);
sm_transform_s transform_inverse(sm_transform_s t);
void transform_point(sm_transform_s a, vec3 b, vec3 out);
void transform_vec3(sm_transform_s a, vec3 b, vec3 out);
sm_transform_s transform_mix(sm_transform_s a, sm_transform_s b, f32 t);

#endif /* SM_MATH_MATH_H */
