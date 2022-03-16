#ifndef SM_MATH_MATH_H
#define SM_MATH_MATH_H

#define CGLM_DEFINE_PRINTS
#include "smTransform.h"
#include "vendor/cglm/include/cglm/cglm.h"

#define vec2_new(x, y)        ((vec2){x, y})
#define vec3_new(x, y, z)     ((vec3){x, y, z})
#define vec4_new(x, y, z, w)  ((vec4){x, y, z, w})
#define ivec4_new(x, y, z, w) ((ivec4){x, y, z, w})


#define mat4_print(M)                                                                                                  \
  printf("\t%s---------\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%"                                                            \
         "7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7.3f|%7.3f|%7.3f|\n\t|%7.3f|%7."                                          \
         "3f|%7.3f|%7.3f|\n\t---------------------------------\n",                                                     \
         #M, M[0][0], M[0][1], M[0][2], M[0][3], M[1][0], M[1][1], M[1][2], M[1][3], M[2][0], M[2][1], M[2][2], M[2][3], M[3][0], M[3][1], M[3][2], M[3][3])


#endif /* SM_MATH_MATH_H */
