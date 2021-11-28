#ifndef VERTEX_H
#define VERTEX_H

#include "shader/attribute.h"
#include "util/common.h"

typedef struct {
  vec3 *positions;
  attribute_s position_attr; // VEC3_EX1

  vec3 *normals;
  attribute_s normal_attr; // VEC3_EX1

  vec2 *tex_coords;
  attribute_s uv_attr; // VEC2_EX1

} vertex_s;

#endif // VERTEX_H
