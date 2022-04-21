#version 120

#define DRAW_VERTEX_COLOR 0
#define DRAW_VERTEX_TEX0 1
#define DRAW_VERTEX_TEX1 2
#define DRAW_VERTEX_TEX2 3
#define DRAW_VERTEX_TEX3 4
#define DRAW_VERTEX_TEX4 5
#define DRAW_VERTEX_TEX5 6
#define DRAW_VERTEX_TEX6 7
#define DRAW_VERTEX_TEX7 8

varying vec4 in_color;
varying vec2 in_tex_coord;
varying float in_tex_id;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform sampler2D u_tex2;
uniform sampler2D u_tex3;
uniform sampler2D u_tex4;
uniform sampler2D u_tex5;
uniform sampler2D u_tex6;
uniform sampler2D u_tex7;

void main() {

  int tex_id = int(in_tex_id);

  if (tex_id == DRAW_VERTEX_COLOR) {
    gl_FragColor = in_color;
  } else if (tex_id == DRAW_VERTEX_TEX0) {
    gl_FragColor = texture2D(u_tex0, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX1) {
    gl_FragColor = texture2D(u_tex1, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX2) {
    gl_FragColor = texture2D(u_tex2, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX3) {
    gl_FragColor = texture2D(u_tex3, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX4) {
    gl_FragColor = texture2D(u_tex4, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX5) {
    gl_FragColor = texture2D(u_tex5, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX6) {
    gl_FragColor = texture2D(u_tex6, in_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX7) {
    gl_FragColor = texture2D(u_tex7, in_tex_coord);
  } else {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
};


