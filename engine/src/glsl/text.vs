#version 120

attribute vec3 position;
// attribute vec3 color;
attribute vec2 tex_coord;
attribute vec3 color;

// varying vec3 out_color;
varying vec2 out_tex_coord;
varying vec3 out_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

  gl_Position = projection * model * vec4(position, 1.0);
  // gl_Position = vec4(position, 1.0);
  // out_color = color;
  out_tex_coord = tex_coord;
  out_color = color;
}

// attribute vec3 position;
// attribute vec2 tex_coord;
// 
// varying vec2 out_tex_coord;
// 
// uniform mat4 projection;
// uniform mat4 model;
// 
// void main() {
// 
//    out_tex_coord = tex_coord;
// 
//    gl_Position = projection * model * vec4(position,1.0);
// };
