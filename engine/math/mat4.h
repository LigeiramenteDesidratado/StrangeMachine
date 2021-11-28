#ifndef MAT4_H
#define MAT4_H

#include "scalar.h"
#include "stdbool.h"
#include "vec3.h"
#include "vec4.h"
#include <stdio.h>

typedef struct {
  union {
        float v[16];
        float vv[4][4];

        struct {
            vec3 right;     // first column
            float w_rigth;
            vec3 up;        // second column
            float w_up;
            vec3 forward;   // third column
            float w_forward;
            vec3 position;  // fourth column
            float w_position;
        };


        struct {
            vec4 right;     // first column
            vec4 up;        // second column
            vec4 forward;   // third column
            vec4 position;  // fourth column
        } vectors;

        // basis vector notation
        struct {
                        // row 1    row 2       row 3       row 4
            /* col 1 */ float xx;   float xy;   float xz;   float xw; // right
            /* col 2 */ float yx;   float yy;   float yz;   float yw; // up
            /* col 3 */ float zx;   float zy;   float zz;   float zw; // forward
            /* col 4 */ float tx;   float ty;   float tz;   float tw; // position
        };

        // column-row notation
        struct {
            float c0r0; float c0r1; float c0r2; float c0r3;
            float c1r0; float c1r1; float c1r2; float c1r3;
            float c2r0; float c2r1; float c2r2; float c2r3;
            float c3r0; float c3r1; float c3r2; float c3r3;
        };

        // row-column notation
        struct {
            float r0c0; float r1c0; float r2c0; float r3c0;
            float r0c1; float r1c1; float r2c1; float r3c1;
            float r0c2; float r1c2; float r2c2; float r3c2;
            float r0c3; float r1c3; float r2c3; float r3c3;
        };
  };

} mat4;

#define mat4_identity()             \
  ((mat4){                          \
        .xx=1, .xy=0, .xz=0, .xw=0, \
        .yx=0, .yy=1, .yz=0, .yw=0, \
        .zx=0, .zy=0, .zz=1, .zw=0, \
        .tx=0, .ty=0, .tz=0, .tw=1  \
  })

#define mat4_new(inXX,  inXY,  inXZ,  inXW, inYX,  inYY,  inYZ,  inYW, inZX,  inZY,  inZZ,  inZW, inTX,  inTY,  inTZ,  inTW) ((mat4) { \
        .xx=inXX, .xy=inXY, .xz=inXZ, .xw=inXW, \
        .yx=inYX, .yy=inYY, .yz=inYZ, .yw=inYW, \
        .zx=inZX, .zy=inZY, .zz=inZZ, .zw=inZW, \
        .tx=inTX, .ty=inTY, .tz=inTZ, .tw=inTW})

#define mat4_print(M)                                                          \
  printf("\t%s---------\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%"                    \
         "7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7."  \
         "3f|%7.3f|%7.3f|\n\t---------------------------------\n",             \
         #M, M.v[0], M.v[1], M.v[2], M.v[3], M.v[4], M.v[5], M.v[6], M.v[7],   \
         M.v[8], M.v[9], M.v[10], M.v[11], M.v[12], M.v[13], M.v[14], M.v[15])

mat4 mat4_look_at(vec3 position, vec3 target, vec3 up);
mat4 mat4_ortho(float l, float r, float b, float t, float n, float f);
mat4 mat4_perspective(float fov, float aspect, float n, float f);
mat4 mat4_frustum(float l, float r, float b, float t, float n, float f);
mat4 mat4_inverse(mat4 m);
float mat4_determinant(mat4 m);
mat4 mat4_transpose(mat4 m);
vec3 mat4_transform_point(mat4 m, vec3 v);
vec3 mat4_transform_vec3(mat4 m, vec3 v);
vec4 mat4_multiply_vec4(mat4 m, vec4 v);
mat4 mat4_mul(mat4 left, mat4 right);
mat4 mat4_scale(mat4 a, float by);
mat4 mat4_add(mat4 a, mat4 b);
bool mat4_not_equal(mat4 a, mat4 b);
bool mat4_is_equal(mat4 a, mat4 b);
mat4 mat4_inverse2(mat4 m);
mat4 mat4_inverse3(mat4 m);

#endif // MAT4_H
