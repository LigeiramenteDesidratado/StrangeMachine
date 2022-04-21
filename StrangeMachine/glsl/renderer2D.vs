#version 120

attribute vec3 position;
attribute vec4 color;
attribute vec2 tex_coord;
attribute float tex_id;

varying vec4 in_color;
varying vec2 in_tex_coord;
varying float in_tex_id;

uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
  gl_Position = u_projection * u_view * vec4(position, 1.0);
  in_color = color;
  in_tex_coord = tex_coord;
  in_tex_id = tex_id;
}
