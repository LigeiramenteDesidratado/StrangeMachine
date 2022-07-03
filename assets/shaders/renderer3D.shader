#version 120

attribute vec3 a_position;
attribute vec2 a_uv;
attribute vec4 a_color;
attribute vec3 a_normal;

uniform mat4 u_model;
// uniform mat4 u_view;
// uniform mat4 u_projection;
uniform mat4 u_pv;

// varying vec3 v_frag_pos;
varying vec2 v_uv;
varying vec4 v_color;
varying vec3 v_normal;

void main() {
  gl_Position = u_pv * u_model * vec4(a_position, 1.0);

  // texture jiggle
  // gl_Position.xy = floor(gl_Position.xy/gl_Position.w*(1./0.009))*0.009*gl_Position.w;

  // frag_pos = vec3(model * vec4(position, 1.0));

  v_uv = a_uv;
  v_color = a_color;
  v_normal = vec3(u_model * vec4(a_normal, 0.0f));
};

// SM FRAGMENT
#version 120

varying vec2 v_uv;
varying vec4 v_color;
varying vec3 v_normal;

uniform sampler2D u_tex0;

void main() {

  /* vec4 diffuse_color = texture2D(u_tex0, v_uv); */

  vec3 n = normalize(v_normal);
  vec3 l = normalize(vec3(0.0, 1.0, 0.0));

  float diffuse_intensity = clamp(dot(n, l), 0.0, 1.0);

  // gl_FragColor = v_color * vec4(1.0, 1.0, 1.0, 1.000000);
  gl_FragColor = diffuse_intensity * v_color;
};


