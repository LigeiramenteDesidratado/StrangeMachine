#ifndef SM_VERTEX_H
#define SM_VERTEX_H

#include "smAttribute.h"
#include "util/common.h"

typedef struct {
  vec3 *positions;
  attribute_s position_attr; // VEC3_KIND

  vec3 *normals;
  attribute_s normal_attr; // VEC3_KIND

  vec2 *tex_coords;
  attribute_s uv_attr; // VEC2_KIND

} vertex_s;

#endif // SM_VERTEX_H
