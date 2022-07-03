#version 120

attribute vec3 a_position;
attribute vec4 a_color;
attribute vec2 a_tex_coord;
attribute float a_tex_id;

varying vec4 v_color;
varying vec2 v_tex_coord;
varying float v_tex_id;

uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
  gl_Position = u_projection * u_view * vec4(a_position, 1.0);
  v_color = a_color;
  v_tex_coord = a_tex_coord;
  v_tex_id = a_tex_id;
}

// SM FRAGMENT
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

varying vec4 v_color;
varying vec2 v_tex_coord;
varying float v_tex_id;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform sampler2D u_tex2;
uniform sampler2D u_tex3;
uniform sampler2D u_tex4;
uniform sampler2D u_tex5;
uniform sampler2D u_tex6;
uniform sampler2D u_tex7;

void main() {

  int tex_id = int(v_tex_id);

  if (tex_id == DRAW_VERTEX_COLOR) {
    gl_FragColor = v_color;
  } else if (tex_id == DRAW_VERTEX_TEX0) {
    gl_FragColor = texture2D(u_tex0, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX1) {
    gl_FragColor = texture2D(u_tex1, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX2) {
    gl_FragColor = texture2D(u_tex2, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX3) {
    gl_FragColor = texture2D(u_tex3, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX4) {
    gl_FragColor = texture2D(u_tex4, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX5) {
    gl_FragColor = texture2D(u_tex5, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX6) {
    gl_FragColor = texture2D(u_tex6, v_tex_coord);
  } else if (tex_id == DRAW_VERTEX_TEX7) {
    gl_FragColor = texture2D(u_tex7, v_tex_coord);
  } else {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
};


