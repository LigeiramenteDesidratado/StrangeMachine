#ifndef SM_MATH_MATH_H
#define SM_MATH_MATH_H

#include "vendor/cglm/include/cglm/cglm.h"

#include "smTransform.h"

#define CMP(X, Y) (fabsf((X) - (Y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(X), fabsf(Y))))

/* Use the same memory for the anonynous structure and the byte array field `data` */
typedef struct sm__vec2 {

  union {

    struct {
      float x, y;
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
      int32_t x;
      int32_t y;
    };

    int32_t v[2];
  };

} sm_ivec2;

#define sm_ivec2_print(V)  printf("%s\n\t%d, %d\n", #V, V.x, V.y);
#define sm_ivec2_new(X, Y) ((sm_ivec2){.x = X, .y = Y})
#define sm_ivec2_zero()    ((sm_ivec2){.x = 0, .y = 0})
#define sm_ivec2_one()     ((sm_ivec2){.x = 1, .y = 1})

/* A vector with 2 uint32_t components */
typedef struct sm__uivec2 {

  union {
    struct {
      uint32_t x;
      uint32_t y;
    };

    uint32_t data[2];
  };

} sm_uivec2;

#define sm_uivec2_print(V)  printf("%s\n\t%d, %d\n", #V, V.x, V.y);
#define sm_uivec2_new(X, Y) ((sm_uivec2){.x = X, .y = Y})
#define sm_uivec2_zero()    ((sm_uivec2){.x = 0, .y = 0})
#define sm_uivec2_one()     ((sm_uivec2){.x = 1, .y = 1})

/* Use the same memory for the anonymous structure and the byte array field `data` */

/* A vector with 3 float components */
typedef struct sm__vec3 {

  union {
    struct {
      float x;
      float y;
      float z;
    };

    struct {
      float r;
      float g;
      float b;
    };

    struct {
      float pitch;
      float yaw;
      float roll;
    };

    vec3 data;
  };

} sm_vec3;

#define sm_vec3_print(V)     printf("%s\n\t%f, %f, %f\n", #V, V.x, V.y, V.z);
#define sm_vec3_new(X, Y, Z) ((sm_vec3){.x = X, .y = Y, .z = Z})
#define sm_vec3_zero()       ((sm_vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f})
#define sm_vec3_one()        ((sm_vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f})
#define sm_vec3_right        sm_vec3_new(1.0f, 0.0f, 0.0f)
#define sm_vec3_up           sm_vec3_new(0.0f, 1.0f, 0.0f)
#define sm_vec3_forward      sm_vec3_new(0.0f, 0.0f, 1.0f)

/* A vector with 4 float components */
typedef CGLM_ALIGN_IF(16) struct sm__vec4 {

  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };

    struct {
      float r;
      float g;
      float b;
      float a;
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
      int32_t x;
      int32_t y;
      int32_t z;
      int32_t w;
    };

    int32_t data[4];
  };

} sm_ivec4;

#define sm_ivec4_print(V)        printf("%s\n\t%d, %d, %d, %d\n", #V, V.x, V.y, V.z, V.w);
#define sm_ivec4_new(X, Y, Z, W) ((sm_ivec4){.x = X, .y = Y, .z = Z, .w = W})
#define sm_ivec4_zero()          ((sm_ivec4){.x = 0, .y = 0, .z = 0, .w = 0})
#define sm_ivec4_one()           ((sm_ivec4){.x = 1, .y = 1, .z = 1, .w = 1})

/* A vector with 4 uint32_t components */
typedef struct sm__uivec4 {

  union {
    struct {
      uint32_t x;
      uint32_t y;
      uint32_t z;
      uint32_t w;
    };

    uint32_t data[4];
  };

} sm_uivec4;

#define sm_uivec4_print(V)        printf("%s\n\t%d, %d, %d, %d\n", #V, V.x, V.y, V.z, V.w);
#define sm_uivec4_new(X, Y, Z, W) ((sm_uivec4){.x = X, .y = Y, .z = Z, .w = W})
#define sm_uivec4_zero()          ((sm_uivec4){.x = 0, .y = 0, .z = 0, .w = 0})
#define sm_uivec4_one()           ((sm_uivec4){.x = 1, .y = 1, .z = 1, .w = 1})

/* A matrix with 4x4 float components */
typedef CGLM_ALIGN_MAT struct sm__mat4 {

  union {

    struct {
      vec4 right;    /* first column */
      vec4 up;       /* second column */
      vec4 forward;  /* third column */
      vec4 position; /* fourth column */
    } vec4;

    struct {
      vec3 right;            /* first column */
      float padding_rigth;   /* w value of right */
      vec3 up;               /* second column */
      float padding_up;      /* w value of up */
      vec3 forward;          /* third column */
      float padding_forward; /* w value of forward */
      vec3 position;         /* fourth column */
      float w_position;

    } vec3;

    /* basis vector notation */
    struct {
      /*          row 1     row 2     row 3     row 4 */
      /* col 1 */ float xx; float xy; float xz; float xw; /* right */
      /* col 2 */ float yx; float yy; float yz; float yw; /* up */
      /* col 3 */ float zx; float zy; float zz; float zw; /* forward */
      /* col 4 */ float tx; float ty; float tz; float tw; /* position */
    };

    /* column-row notation */
    struct {
      float c0r0; float c0r1; float c0r2; float c0r3;
      float c1r0; float c1r1; float c1r2; float c1r3;
      float c2r0; float c2r1; float c2r2; float c2r3;
      float c3r0; float c3r1; float c3r2; float c3r3;
    };

    /* row-column notation */
    struct {
      float r0c0; float r1c0; float r2c0; float r3c0;
      float r0c1; float r1c1; float r2c1; float r3c1;
      float r0c2; float r1c2; float r2c2; float r3c2;
      float r0c3; float r1c3; float r2c3; float r3c3;
    };

    mat4 data;
    float float16[16];
  };

} sm_mat4;


#define sm_mat4_print(M)                                                                                               \
  printf("\t%s---------\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%"                                                            \
         "7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7."                                          \
         "3f|%7.3f|%7.3f|\n\t---------------------------------\n",                                                     \
         #M, M.v[0], M.v[1], M.v[2], M.v[3], M.v[4], M.v[5], M.v[6], M.v[7], M.v[8], M.v[9], M.v[10], M.v[11],         \
         M.v[12], M.v[13], M.v[14], M.v[15])

#define sm_mat4_new(inXX, inXY, inXZ, inXW, inYX, inYY, inYZ, inYW, inZX, inZY, inZZ, inZW, inTX, inTY, inTZ, inTW) \
  ((sm_mat4) { \
   .xx=inXX, .xy=inXY, .xz=inXZ, .xw=inXW, \
   .yx=inYX, .yy=inYY, .yz=inYZ, .yw=inYW, \
   .zx=inZX, .zy=inZY, .zz=inZZ, .zw=inZW, \
   .tx=inTX, .ty=inTY, .tz=inTZ, .tw=inTW})

#define sm_mat4_zero()                                                                                                 \
  ((sm_mat4) { \
   .xx=0.0f, .xy=0.0f, .xz=0.0f, .xw=0.0f, \
   .yx=0.0f, .yy=0.0f, .yz=0.0f, .yw=0.0f, \
   .zx=0.0f, .zy=0.0f, .zz=0.0f, .zw=0.0f, \
   .tx=0.0f, .ty=0.0f, .tz=0.0f, .tw=0.0f})

#define sm_mat4_identity()                                                                                             \
  ((sm_mat4) { \
   .xx=1.0f, .xy=0.0f, .xz=0.0f, .xw=0.0f, \
   .yx=0.0f, .yy=1.0f, .yz=0.0f, .yw=0.0f, \
   .zx=0.0f, .zy=0.0f, .zz=1.0f, .zw=0.0f, \
   .tx=0.0f, .ty=0.0f, .tz=0.0f, .tw=1.0f})

#endif /* SM_MATH_MATH_H */
