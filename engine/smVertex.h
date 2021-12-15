#ifndef SM_VERTEX_H
#define SM_VERTEX_H

#include "smAttribute.h"

typedef struct {
  vec3 *positions;
  attribute_s position_attr; // VEC3_EX1

  vec3 *normals;
  attribute_s normal_attr; // VEC3_EX1

  vec2 *tex_coords;
  attribute_s uv_attr; // VEC2_EX1

} vertex_s;

#endif // SM_VERTEX_H
