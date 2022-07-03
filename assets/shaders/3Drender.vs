#version 120

attribute vec3 position;
attribute vec4 color;
attribute vec3 normal;
attribute vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 norm;
varying vec4 col;
varying vec2 uv;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);

  // texture jiggle
  // gl_Position.xy = floor(gl_Position.xy/gl_Position.w*(1./0.009))*0.009*gl_Position.w;

  // frag_pos = vec3(model * vec4(position, 1.0));
  norm = vec3(model * vec4(normal, 0.0f));
  uv = tex_coord;
};

