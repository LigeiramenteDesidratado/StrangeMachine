#include "text/text.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/transform.h"
#include "shader/attribute.h"
#include "shader/index_buffer.h"
#include "shader/shader.h"
#include "shader/texture.h"
#include "shader/uniform.h"
#include "shader_program.h"
#include "stb_ds/stb_ds.h"
#include "util/common.h"

#define GLYPH_W 7
#define GLYPH_H 7

const struct text_attr text_attr_locs = {
    .position = 1, .tex_coord = 2, .color = 3};

typedef struct {

  texture_s texture;

  vec3 *positions;
  attribute_s position_attr; // VEC3_EX1

  vec2 *tex_coords;
  attribute_s uv_attr; // VEC2_EX1

  vec3 *color;
  attribute_s color_attr; // VEC2_EX1

  /* index_buffer_s index_buffer; */
  /* uint32_t *indices; */

} text_s;

text_s TEXT = {0};

void text_init(void) {

  TEXT.position_attr = attribute_new();
  if (!attribute_ctor(&TEXT.position_attr, VEC3_EX1))
    exit(1);

  TEXT.uv_attr = attribute_new();
  if (!attribute_ctor(&TEXT.uv_attr, VEC2_EX1))
    exit(1);

  TEXT.color_attr = attribute_new();
  if (!attribute_ctor(&TEXT.color_attr, VEC3_EX1))
    exit(1);

  TEXT.positions = NULL;
  TEXT.tex_coords = NULL;
  TEXT.color = NULL;

  texture_s texture = texture_new();
  if (!texture_ctor(&texture, "engine/src/text/fonts.png")) {
    exit(1);
  }
  TEXT.texture = texture;

}

void text_draw(vec2 dest, int w, vec3 color, char *format, ...) {

  va_list argsf;

  static char text_buffer[1024];
  memset(&text_buffer, '\0', sizeof(text_buffer));

  va_start(argsf, format);
  vsprintf(text_buffer, format, argsf);
  va_end(argsf);

  int len = strlen(text_buffer);
  float initx = dest.x;
  float srcY = 0.0f;
  float srcX = 0.0f;

  int space_bX = 0;
  int space_bY = 2;
  uint32_t width = TEXT.texture.width;
  uint32_t height = TEXT.texture.height;

  for (int i = 0; i < len; i++) {
    char c = text_buffer[i];
    if (c == '\n') {
      dest.x = initx;
      dest.y += GLYPH_H + space_bY;
      continue;
    }

    if (c >= ' ' && c <= '~') {
      if (c == ' ') {
        dest.x += GLYPH_W + space_bX;
        continue;
      }

      srcX = (c - ' ') * GLYPH_W;

      arrput(TEXT.positions, vec3_new(dest.x, dest.y, 0.0f));  // top left
      arrput(TEXT.positions, vec3_new(dest.x, dest.y+GLYPH_H, 0.0f));  // bottom left
      arrput(TEXT.positions, vec3_new(dest.x+GLYPH_W, dest.y, 0.0f)); //top right

      arrput(TEXT.positions, vec3_new(dest.x+GLYPH_W, dest.y+GLYPH_H, 0.0f));  // bottom right
      arrput(TEXT.positions, vec3_new(dest.x+GLYPH_W, dest.y, 0.0f));  // top right
      arrput(TEXT.positions, vec3_new(dest.x, dest.y+GLYPH_H, 0.0f));  // bottom left

      // OpenGL does store texture "upside-down", so flip it
      arrput(TEXT.tex_coords, vec2_new(srcX/width, (srcY+GLYPH_H)/height)); // top left
      arrput(TEXT.tex_coords, vec2_new(srcX/width, srcY/height)); // bottom left
      arrput(TEXT.tex_coords, vec2_new((srcX+GLYPH_W)/width , (srcY+GLYPH_H)/height)); // top right

      arrput(TEXT.tex_coords, vec2_new((srcX+GLYPH_W)/width, srcY/height));  // bottom right
      arrput(TEXT.tex_coords, vec2_new((srcX+GLYPH_W)/width , (srcY+GLYPH_H)/height)); // top right
      arrput(TEXT.tex_coords, vec2_new(srcX/width, srcY/height)); // bottom left


      arrput(TEXT.color, color);
      arrput(TEXT.color, color);
      arrput(TEXT.color, color);

      arrput(TEXT.color, color);
      arrput(TEXT.color, color);
      arrput(TEXT.color, color);

      if (dest.x + GLYPH_W + space_bX >= w) {
        dest.x = initx;
        dest.y += GLYPH_H + space_bY;
      } else {
        dest.x += GLYPH_W + space_bX;
      }
    }
  }
}

void text_flush(void) {

  if (arrlenu(TEXT.positions) > 0) attribute_set(&TEXT.position_attr, TEXT.positions, arrlenu(TEXT.positions), GL_STREAM_DRAW);
  if (arrlenu(TEXT.tex_coords) > 0) attribute_set(&TEXT.uv_attr, TEXT.tex_coords, arrlenu(TEXT.tex_coords), GL_STREAM_DRAW);
  if (arrlenu(TEXT.tex_coords) > 0) attribute_set(&TEXT.color_attr, TEXT.color, arrlenu(TEXT.color), GL_STREAM_DRAW);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  shader_bind(SHADERS[TEXT_SHADER_EX7]);

  static int angle = 360;
  if (angle++ >= 360)
    angle = 0;

  transform_s transf = transform_zero();
  /* transf.rotation = quat_angle_axis(DEG2RAD * angle, vec3_new(0.0f, 1.0f, 0.0f)); */
  transf.scale = vec3_new(3, 3, 3);
  /* transf.position = vec3_new(-420, 250, 0); */
  mat4 model = transform_to_mat4(transf);
  /* model = mat4_identity(); */

  uniform_set_value(glGetUniformLocation(SHADERS[TEXT_SHADER_EX7], "model"), model);

  attribute_bind_to(&TEXT.position_attr, text_attr_locs.position);
  attribute_bind_to(&TEXT.uv_attr, text_attr_locs.tex_coord);
  attribute_bind_to(&TEXT.color_attr, text_attr_locs.color);
  texture_set(&TEXT.texture, glGetUniformLocation(SHADERS[TEXT_SHADER_EX7], "tex0"), 0);


  glDrawArrays(GL_TRIANGLES, 0, arrlen(TEXT.positions));


  attribute_unbind_from(&TEXT.color_attr, text_attr_locs.color);
  attribute_unbind_from(&TEXT.uv_attr, text_attr_locs.tex_coord);
  attribute_unbind_from(&TEXT.position_attr, text_attr_locs.position);
  texture_unset(0);
  shader_unbind();
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); */
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  arrsetlen(TEXT.positions, 0);
  arrsetlen(TEXT.tex_coords, 0);
  arrsetlen(TEXT.color, 0);

}

void text_tear_down(void) {

  attribute_dtor(&TEXT.position_attr);
  arrfree(TEXT.positions);

  attribute_dtor(&TEXT.uv_attr);
  arrfree(TEXT.tex_coords);
}
