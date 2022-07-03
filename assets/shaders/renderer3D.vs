#version 120

attribute vec3 a_position;
attribute vec2 a_tex_coord;
attribute vec4 a_color;
attribute vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

// varying vec3 v_frag_pos;
varying vec2 v_uv;
varying vec4 v_color;
varying vec3 v_normal;

void main() {
  gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

  // texture jiggle
  gl_Position.xy = floor(gl_Position.xy/gl_Position.w*(1./0.009))*0.009*gl_Position.w;

  // frag_pos = vec3(model * vec4(position, 1.0));

  v_uv = a_tex_coord;
  v_color = a_color;
  v_normal = vec3(u_model * vec4(a_normal, 0.0f));
};

