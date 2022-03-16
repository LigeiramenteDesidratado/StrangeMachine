#include "util/common.h"

#include "data/array.h"

#include "smAttribute.h"
#include "smMem.h"
#include "smMesh.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smShapes.h"
#include "smUniform.h"

typedef struct {

  vec3 *positions;
  attribute_s position_attr; // VEC3_KIND

  vec3 *colors;
  attribute_s color_attr; // VEC3_KIND

} debug_s;

static debug_s DEBUG = {0};

bool debug_init() {

  DEBUG.position_attr = attribute_new();
  if (!attribute_ctor(&DEBUG.position_attr, VEC3_KIND))
    return false;

  DEBUG.color_attr = attribute_new();
  if (!attribute_ctor(&DEBUG.color_attr, VEC3_KIND))
    return false;

  DEBUG.colors = (vec3 *)SM_ARRAY_NEW_EMPTY();
  DEBUG.positions = (vec3 *)SM_ARRAY_NEW_EMPTY();

  return true;
}

void debug_tear_down(void) {
  attribute_dtor(&DEBUG.position_attr);
  SM_ARRAY_DTOR(DEBUG.positions);

  attribute_dtor(&DEBUG.color_attr);
  SM_ARRAY_DTOR(DEBUG.colors);
}

// Draw aabb wires.
void debug_draw_aabb(bounding_box_s aabb, vec3 color) {

  float width = fabsf(aabb.max[0] - aabb.min[0]);
  float height = fabsf(aabb.max[1] - aabb.min[1]);
  float length = fabsf(aabb.max[2] - aabb.min[2]);

  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  vec3 cube[24] = {

      // Front Face -----------------------------------------------------
      // Bottom Line
      {x - width / 2, y - height / 2, z + length / 2}, // Bottom Left
      {x + width / 2, y - height / 2, z + length / 2}, // Bottom Right

      // Left Line
      {x + width / 2, y - height / 2, z + length / 2}, // Bottom Right
      {x + width / 2, y + height / 2, z + length / 2}, // Top Right

      // Top Line
      {x + width / 2, y + height / 2, z + length / 2}, // Top Right
      {x - width / 2, y + height / 2, z + length / 2}, // Top Left

      // Right Line
      {x - width / 2, y + height / 2, z + length / 2}, // Top Left
      {x - width / 2, y - height / 2, z + length / 2}, // Bottom Left

      // Back Face ------------------------------------------------------
      // Bottom Line
      {x - width / 2, y - height / 2, z - length / 2}, // Bottom Left
      {x + width / 2, y - height / 2, z - length / 2}, // Bottom Right

      // Left Line
      {x + width / 2, y - height / 2, z - length / 2}, // Bottom Right
      {x + width / 2, y + height / 2, z - length / 2}, // Top Right

      // Top Line
      {x + width / 2, y + height / 2, z - length / 2}, // Top Right
      {x - width / 2, y + height / 2, z - length / 2}, // Top Left

      // Right Line
      {x - width / 2, y + height / 2, z - length / 2}, // Top Left
      {x - width / 2, y - height / 2, z - length / 2}, // Bottom Left

      // Top Face -------------------------------------------------------
      // Left Line
      {x - width / 2, y + height / 2, z + length / 2}, // Top Left Front
      {x - width / 2, y + height / 2, z - length / 2}, // Top Left Back

      // Right Line
      {x + width / 2, y + height / 2, z + length / 2}, // Top Right Front
      {x + width / 2, y + height / 2, z - length / 2}, // Top Right Back

      // Bottom Face  ---------------------------------------------------
      // Left Line
      {x - width / 2, y - height / 2, z + length / 2}, // Top Left Front
      {x - width / 2, y - height / 2, z - length / 2}, // Top Left Back

      // Right Line
      {x + width / 2, y - height / 2, z + length / 2}, // Top Right Front
      {x + width / 2, y - height / 2, z - length / 2}, // Top Right Back

  };

  vec3 colors[24];
  for (uint8_t i = 0; i < 24; ++i) {
    glm_vec3_copy(color, colors[i]);
  }

  transform_s transform = transform_zero();
  transform.position[0] = aabb.min[0] + width / 2.0f;
  transform.position[1] = aabb.min[1] + height / 2.0f;
  transform.position[2] = aabb.min[2] + length / 2.0f;

  shader_bind(SHADERS[DEBUG_SHADER]);
  attribute_set(&DEBUG.position_attr, cube, 24, GL_STATIC_DRAW);
  attribute_set(&DEBUG.color_attr, colors, 24, GL_STATIC_DRAW);

  /* mat4 model = mat4_identity(); */
  /* uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model); */
  mat4 model;
  transform_to_mat4(transform, model);
  uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model);

  attribute_bind_to(&DEBUG.position_attr, 1);
  attribute_bind_to(&DEBUG.color_attr, 2);

  glDrawArrays(GL_LINES, 0, 24);

  attribute_unbind_from(&DEBUG.color_attr, 2);
  attribute_unbind_from(&DEBUG.position_attr, 1);

  shader_unbind();
}

void vec3_orthonorm(vec3 left, vec3 up, const vec3 v) {
  float lenSqr, invLen;
  if (fabs(v[2]) > 0.7f) {
    lenSqr = v[1] * v[1] + v[2] * v[2];
    /* invLen = rev_sqrt(lenSqr); */
    invLen = 1.0f / sqrt(lenSqr);

    up[0] = 0.0f;
    up[1] = v[2] * invLen;
    up[2] = -v[1] * invLen;

    left[0] = lenSqr * invLen;
    left[1] = -v[0] * up[2];
    left[2] = v[0] * up[1];
  } else {
    lenSqr = v[0] * v[0] + v[1] * v[1];
    /* invLen = rev_sqrt(lenSqr); */
    invLen = 1.0f / sqrt(lenSqr);

    left[0] = -v[1] * invLen;
    left[1] = v[0] * invLen;
    left[2] = 0.0f;

    up[0] = -v[2] * left[1];
    up[1] = v[2] * left[0];
    up[2] = lenSqr * invLen;
  }
}

// The translation is already stored in the capsule tip and base component.
// Maybe I fix this later in favor of using transformation_s
void debug_draw_capsule(capsule_s c) {

  vec3 *vert = NULL;
  vec3 *colors = NULL;

  SM_ARRAY_SET_SIZE(vert, 10800);
  SM_ARRAY_SET_SIZE(colors, 10800);

  // Since we only have a description of the capsule we need to generate
  // all the vertices of the capsule
  const uint16_t step_size = 360 / 12;
  vec3 up, right, forward;
  vec3 from, to;
  vec3 lastf, lastt;

  glm_vec3_copy(vec3_new(c.base[0], c.base[1] + c.radius, c.base[2]), from);
  glm_vec3_copy(vec3_new(c.tip[0], c.tip[1] - c.radius, c.tip[2]), to);

  glm_vec3_sub(to, from, forward);
  glm_vec3_normalize(forward);

  vec3_orthonorm(right, up, forward);

  glm_vec3_scale(up, c.radius, lastf);
  glm_vec3_add(to, lastf, lastt);
  glm_vec3_add(from, lastf, lastf);

  uint16_t offset = 0;
  for (uint16_t i = step_size; i <= 360; i += step_size) {

    vec3 ax, ay, pf, pt, tmp;

    glm_vec3_scale(right, sinf(glm_rad(i)), ax);
    glm_vec3_scale(up, cosf(glm_rad(i)), ay);

    glm_vec3_add(ax, ay, tmp);
    glm_vec3_scale(tmp, c.radius, pt);
    glm_vec3_add(pt, to, pt);
    glm_vec3_scale(tmp, c.radius, pf);
    glm_vec3_add(pf, from, pf);

    glm_vec3_copy(lastf, vert[offset++]);
    glm_vec3_copy(pf, vert[offset++]);
    glm_vec3_copy(lastt, vert[offset++]);
    glm_vec3_copy(pt, vert[offset++]);
    glm_vec3_copy(pf, vert[offset++]);
    glm_vec3_copy(pt, vert[offset++]);

    glm_vec3_copy(pf, lastf);
    glm_vec3_copy(pt, lastt);

    vec3 prevt, prevf;

    glm_vec3_scale(tmp, c.radius, prevt);
    glm_vec3_add(prevt, from, prevf);
    glm_vec3_add(prevt, to, prevt);

    for (int j = 1; j < 180 / step_size; j++) {

      float ta = (float)j * step_size;
      float fa = 360 - (float)(j * step_size);
      vec3 t;

      glm_vec3_scale(forward, sinf(glm_rad(ta)), ax);
      glm_vec3_scale(tmp, cosf(glm_rad(ta)), ay);

      glm_vec3_add(ax, ay, t);
      glm_vec3_scale(t, c.radius, pf);
      glm_vec3_add(pf, to, pf);

      glm_vec3_copy(pf, vert[offset++]);
      glm_vec3_copy(prevt, vert[offset++]);

      glm_vec3_copy(pf, prevt);

      glm_vec3_scale(forward, sinf(glm_rad(fa)), ax);
      glm_vec3_scale(tmp, cosf(glm_rad(fa)), ay);

      glm_vec3_add(ax, ay, t);
      glm_vec3_scale(t, c.radius, pf);
      glm_vec3_add(pf, from, pf);

      glm_vec3_copy(pf, vert[offset++]);
      glm_vec3_copy(prevf, vert[offset++]);

      glm_vec3_copy(pf, prevf);
    }
  }

  size_t s = SM_ARRAY_SIZE(vert);
  SM_ARRAY_SET_SIZE(colors, s);
  for (size_t i = 0; i < s; ++i) {
    glm_vec3_copy(vec3_new(0.6f, 0.5f, 0.7f), colors[i]);
  }

  shader_bind(SHADERS[DEBUG_SHADER]);
  attribute_set(&DEBUG.position_attr, vert, s, GL_STATIC_DRAW);
  attribute_set(&DEBUG.color_attr, colors, s, GL_STATIC_DRAW);

  mat4 model = GLM_MAT4_IDENTITY_INIT;
  uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model);

  attribute_bind_to(&DEBUG.position_attr, 1);
  attribute_bind_to(&DEBUG.color_attr, 2);

  glDrawArrays(GL_LINES, 0, (GLsizei)s);

  attribute_unbind_from(&DEBUG.color_attr, 2);
  attribute_unbind_from(&DEBUG.position_attr, 1);

  shader_unbind();

  SM_ARRAY_DTOR(vert);
  SM_ARRAY_DTOR(colors);
}

void debug_draw_line(vec3 from, vec3 to, vec3 color) {

  vec3 line[2] = {
      {from[0], from[1], from[2]},
      {to[0], to[1], to[2]},
  };

  vec3 colors[2] = {
      {color[0], color[1], color[2]},
      {color[0], color[1], color[2]},
  };

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader_bind(SHADERS[DEBUG_SHADER]);
  glLineWidth(3);
  attribute_set(&DEBUG.position_attr, line, 2, GL_STATIC_DRAW);
  attribute_set(&DEBUG.color_attr, colors, 2, GL_STATIC_DRAW);

  mat4 model = GLM_MAT4_IDENTITY_INIT;
  uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model);

  attribute_bind_to(&DEBUG.position_attr, 1);
  attribute_bind_to(&DEBUG.color_attr, 2);

  glDrawArrays(GL_LINES, 0, 2);

  attribute_unbind_from(&DEBUG.color_attr, 2);
  attribute_unbind_from(&DEBUG.position_attr, 1);

  shader_unbind();

  glLineWidth(1);
  glDisable(GL_BLEND);
}

// draw sphere wireframe
// void debug_draw_sphere(sphere_s s1, vec3 c) {
//
//   // uv shpere mesh
//   vec3 *vert = (vec3 *)SM_ARRAY_NEW_EMPTY();
//   vec3 *colors = (vec3 *)SM_ARRAY_NEW_EMPTY();
//
//   float x, y, z, xy;
//
//   int slices = 20;
//   int stacks = 20;
//
//   float sectorStep = 2 * GLM_PI / slices;
//   float stackStep = GLM_PI / stacks;
//   float sectorAngle, stackAngle;
//
//   for (int i = 0; i <= stacks; ++i) {
//     stackAngle = GLM_PI / 2 - i * stackStep;
//     xy = s1.radius * cosf(stackAngle);
//     z = s1.radius * sinf(stackAngle);
//
//     for (int j = 0; j <= slices; ++j) {
//       sectorAngle = j * sectorStep;
//
//       x = xy * cosf(sectorAngle);
//       y = xy * sinf(sectorAngle);
//       SM_ARRAY_PUSH(vert, vec3_new(x, y, z));
//     }
//   }
//
//   uint32_t *indices = (uint32_t *)SM_ARRAY_NEW_EMPTY();
//   uint32_t *lineIndices = (uint32_t *)SM_ARRAY_NEW_EMPTY();
//   int k1, k2;
//   for (int i = 0; i < stacks; ++i) {
//     k1 = i * (slices + 1);
//     k2 = k1 + slices + 1;
//
//     for (int j = 0; j < slices; ++j, ++k1, ++k2) {
//
//       if (i != 0) {
//         SM_ARRAY_PUSH(indices, k1);
//         SM_ARRAY_PUSH(indices, k2);
//         SM_ARRAY_PUSH(indices, k1 + 1);
//       }
//
//       if (i != (stacks - 1)) {
//         SM_ARRAY_PUSH(indices, k1 + 1);
//         SM_ARRAY_PUSH(indices, k2);
//         SM_ARRAY_PUSH(indices, k2 + 1);
//       }
//
//       SM_ARRAY_PUSH(lineIndices, k1);
//       SM_ARRAY_PUSH(lineIndices, k2);
//       if (i != 0) {
//         SM_ARRAY_PUSH(lineIndices, k1);
//         SM_ARRAY_PUSH(lineIndices, k1 + 1);
//       }
//     }
//   }
//
//   size_t s = SM_ARRAY_SIZE(vert);
//   SM_ARRAY_SET_SIZE(colors, s);
//   for (size_t i = 0; i < s; ++i) {
//     glm_vec3_copy(c, colors[i]);
//   }
//
//   shader_bind(SHADERS[DEBUG_SHADER]);
//   attribute_set(&DEBUG.position_attr, vert, s, GL_STATIC_DRAW);
//   attribute_set(&DEBUG.color_attr, colors, s, GL_STATIC_DRAW);
//
//   transform_s t = transform_zero();
//   glm_vec3_copy(s1.center, t.position);
//   mat4 model;
//   transform_to_mat4(t, model);
//   uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model);
//
//   attribute_bind_to(&DEBUG.position_attr, 1);
//   attribute_bind_to(&DEBUG.color_attr, 2);
//
//   glDrawElements(GL_TRIANGLES, (GLsizei)SM_ARRAY_SIZE(indices), GL_UNSIGNED_INT, indices);
//
//   attribute_unbind_from(&DEBUG.color_attr, 2);
//   attribute_unbind_from(&DEBUG.position_attr, 1);
//
//   shader_unbind();
//
//   SM_ARRAY_DTOR(vert);
//   SM_ARRAY_DTOR(colors);
//   SM_ARRAY_DTOR(indices);
//   SM_ARRAY_DTOR(lineIndices);
// }
