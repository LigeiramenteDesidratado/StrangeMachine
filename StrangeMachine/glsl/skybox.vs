#version 120

attribute vec3 position;

uniform mat4 view;
uniform mat4 projection;

varying vec3 frag_pos;

void main() {

  frag_pos = position;
  gl_Position = projection * view * vec4(position, 1.0);
};

