#include "smAttribute.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smShapes.h"
#include "smUniform.h"
#include "util/common.h"

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

  return true;
}

void debug_tear_down(void) {
  attribute_dtor(&DEBUG.position_attr);
  arrfree(DEBUG.positions);

  attribute_dtor(&DEBUG.color_attr);
  arrfree(DEBUG.colors);
}

// Draw aabb wires.
void debug_draw_aabb(bounding_box_s aabb, vec3 color) {

  float width = fabsf(aabb.max.x - aabb.min.x);
  float height = fabsf(aabb.max.y - aabb.min.y);
  float length = fabsf(aabb.max.z - aabb.min.z);

  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  vec3 cube[24] = {

      // Front Face -----------------------------------------------------
      // Bottom Line
      vec3_new(x - width / 2, y - height / 2, z + length / 2), // Bottom Left
      vec3_new(x + width / 2, y - height / 2, z + length / 2), // Bottom Right

      // Left Line
      vec3_new(x + width / 2, y - height / 2, z + length / 2), // Bottom Right
      vec3_new(x + width / 2, y + height / 2, z + length / 2), // Top Right

      // Top Line
      vec3_new(x + width / 2, y + height / 2, z + length / 2), // Top Right
      vec3_new(x - width / 2, y + height / 2, z + length / 2), // Top Left

      // Right Line
      vec3_new(x - width / 2, y + height / 2, z + length / 2), // Top Left
      vec3_new(x - width / 2, y - height / 2, z + length / 2), // Bottom Left

      // Back Face ------------------------------------------------------
      // Bottom Line
      vec3_new(x - width / 2, y - height / 2, z - length / 2), // Bottom Left
      vec3_new(x + width / 2, y - height / 2, z - length / 2), // Bottom Right

      // Left Line
      vec3_new(x + width / 2, y - height / 2, z - length / 2), // Bottom Right
      vec3_new(x + width / 2, y + height / 2, z - length / 2), // Top Right

      // Top Line
      vec3_new(x + width / 2, y + height / 2, z - length / 2), // Top Right
      vec3_new(x - width / 2, y + height / 2, z - length / 2), // Top Left

      // Right Line
      vec3_new(x - width / 2, y + height / 2, z - length / 2), // Top Left
      vec3_new(x - width / 2, y - height / 2, z - length / 2), // Bottom Left

      // Top Face -------------------------------------------------------
      // Left Line
      vec3_new(x - width / 2, y + height / 2, z + length / 2), // Top Left Front
      vec3_new(x - width / 2, y + height / 2, z - length / 2), // Top Left Back

      // Right Line
      vec3_new(x + width / 2, y + height / 2, z + length / 2), // Top Right Front
      vec3_new(x + width / 2, y + height / 2, z - length / 2), // Top Right Back

      // Bottom Face  ---------------------------------------------------
      // Left Line
      vec3_new(x - width / 2, y - height / 2, z + length / 2), // Top Left Front
      vec3_new(x - width / 2, y - height / 2, z - length / 2), // Top Left Back

      // Right Line
      vec3_new(x + width / 2, y - height / 2, z + length / 2), // Top Right Front
      vec3_new(x + width / 2, y - height / 2, z - length / 2), // Top Right Back

  };

  vec3 colors[24];
  for (uint8_t i = 0; i < 24; ++i) {
    colors[i] = color;
  }

  transform_s transform = transform_zero();
  transform.position =
      (vec3){.x = aabb.min.x + width / 2.0f, .y = aabb.min.y + height / 2.0f, .z = aabb.min.z + length / 2.0f};

  shader_bind(SHADERS[DEBUG_SHADER]);
  attribute_set(&DEBUG.position_attr, cube, 24, GL_STATIC_DRAW);
  attribute_set(&DEBUG.color_attr, colors, 24, GL_STATIC_DRAW);

  /* mat4 model = mat4_identity(); */
  /* uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model); */
  mat4 model = transform_to_mat4(transform);
  uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model);

  attribute_bind_to(&DEBUG.position_attr, 1);
  attribute_bind_to(&DEBUG.color_attr, 2);

  glDrawArrays(GL_LINES, 0, 24);

  attribute_unbind_from(&DEBUG.color_attr, 2);
  attribute_unbind_from(&DEBUG.position_attr, 1);

  shader_unbind();
}

// The translation is already stored in the capsule tip and base component.
// Maybe I fix this later in favor of using transformation_s
void debug_draw_capsule(capsule_s c) {

  vec3 *vert = NULL;
  vec3 *colors = NULL;

  // Since we only have a description of the capsule we need to generate
  // all the vertices of the capsule
  const uint16_t step_size = 360 / 12;
  vec3 up, right, forward;
  vec3 from, to;
  vec3 lastf, lastt;

  from = vec3_new(c.base.x, c.base.y + c.radius, c.base.z);
  to = vec3_new(c.tip.x, c.tip.y - c.radius, c.tip.z);

  forward = vec3_sub(to, from);

  forward = vec3_norm(forward);

  vec3_orthonorm(&right, &up, forward);

  lastf = vec3_scale(up, c.radius);
  lastt = vec3_add(to, lastf);
  lastf = vec3_add(from, lastf);

  for (uint16_t i = step_size; i <= 360; i += step_size) {

    vec3 ax, ay, pf, pt, tmp;

    ax = vec3_scale(right, sinf(i * DEG2RAD));
    ay = vec3_scale(up, cosf(i * DEG2RAD));

    tmp = vec3_add(ax, ay);

    pt = vec3_add(vec3_scale(tmp, c.radius), to);
    pf = vec3_add(vec3_scale(tmp, c.radius), from);

    arrput(vert, lastf);
    arrput(vert, pf);
    arrput(vert, lastt);
    arrput(vert, pt);
    arrput(vert, pf);
    arrput(vert, pt);

    lastf = pf;
    lastt = pt;

    vec3 prevt, prevf;

    prevt = vec3_scale(tmp, c.radius);
    prevf = vec3_add(prevt, from);
    prevt = vec3_add(prevt, to);

    for (int j = 1; j < 180 / step_size; j++) {

      float ta = (float)j * step_size;
      float fa = 360 - (float)(j * step_size);
      vec3 t;

      ax = vec3_scale(forward, sinf(ta * DEG2RAD));
      ay = vec3_scale(tmp, cosf(ta * DEG2RAD));

      t = vec3_add(ax, ay);
      pf = vec3_add(vec3_scale(t, c.radius), to);

      arrput(vert, pf);
      arrput(vert, prevt);

      prevt = pf;

      ax = vec3_scale(forward, sinf(fa * DEG2RAD));
      ay = vec3_scale(tmp, cosf(fa * DEG2RAD));

      t = vec3_add(ax, ay);
      pf = vec3_add(vec3_scale(t, c.radius), from);

      arrput(vert, pf);
      arrput(vert, prevf);

      prevf = pf;
    }
  }

  size_t s = arrlenu(vert);
  arrsetlen(colors, s);
  for (size_t i = 0; i < s; ++i) {
    colors[i] = vec3_new(1.0f, 1.0f, 1.0f);
  }

  shader_bind(SHADERS[DEBUG_SHADER]);
  attribute_set(&DEBUG.position_attr, vert, s, GL_STATIC_DRAW);
  attribute_set(&DEBUG.color_attr, colors, s, GL_STATIC_DRAW);

  mat4 model = mat4_identity();
  uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER], "model"), model);

  attribute_bind_to(&DEBUG.position_attr, 1);
  attribute_bind_to(&DEBUG.color_attr, 2);

  glDrawArrays(GL_LINES, 0, (GLsizei)s);

  attribute_unbind_from(&DEBUG.color_attr, 2);
  attribute_unbind_from(&DEBUG.position_attr, 1);

  shader_unbind();

  arrfree(vert);
  arrfree(colors);
}

void debug_draw_line(vec3 from, vec3 to, vec3 color) {

  vec3 line[2] = {
      from,
      to,
  };

  vec3 colors[2] = {
      color,
      color,
  };

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader_bind(SHADERS[DEBUG_SHADER]);
  glLineWidth(3);
  attribute_set(&DEBUG.position_attr, line, 2, GL_STATIC_DRAW);
  attribute_set(&DEBUG.color_attr, colors, 2, GL_STATIC_DRAW);

  mat4 model = mat4_identity();
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
