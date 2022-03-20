#version 120

attribute vec3 position;
attribute vec4 color;
attribute vec2 tex_coord;
attribute float tex_id;

varying vec4 in_color;
varying vec2 in_tex_coord;
varying float in_tex_id;

void main() {
  gl_Position = vec4(position, 1.0);
  in_color = color;
  in_tex_coord = tex_coord;
  in_tex_id = tex_id;
}
