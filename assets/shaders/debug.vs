#version 120

attribute vec3 position;
attribute vec3 color;

varying vec3 frag_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);

  // texture jiggle
  // gl_Position.xy = floor(gl_Position.xy/gl_Position.w*(1./0.009))*0.009*gl_Position.w;
  frag_color = color;
}
