#version 120
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

attribute vec3 position;
attribute vec3 normal;
attribute vec2 tex_coord;
attribute vec4 weight;
attribute ivec4 joint;

uniform mat4 animated[120];

varying vec3 norm;
varying vec3 frag_pos;
varying vec2 uv;

void main() {

  mat4 skin = animated[int(joint.x)] * weight.x +
  animated[int(joint.y)] * weight.y +
  animated[int(joint.z)] * weight.z +
  animated[int(joint.w)] * weight.w;

  gl_Position = projection * view * model * skin * vec4(position, 1.0);
  // gl_Position.xy = floor(gl_Position.xy/gl_Position.w*(1./0.009))*0.009*gl_Position.w;

  frag_pos = vec3(model * skin * vec4(position, 1.0));
  norm = vec3(model * skin * vec4(normal, 0.0f));
  uv = tex_coord;
}
