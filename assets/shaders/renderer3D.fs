#version 120

varying vec2 v_uv;
varying vec4 v_color;
varying vec3 v_normal;

uniform sampler2D u_tex0;

void main() {

  /* vec4 diffuse_color = texture2D(u_tex0, v_uv); */

  vec3 n = normalize(v_normal);
  vec3 l = normalize(vec3(1.0, 1.0, 1.0));

  float diffuse_intensity = clamp(dot(n, l), 0.0, 1.0);

  /* gl_FragColor = diffuse_intensity * vec4(0.521569, 0.862745, 0.521569, 1.000000); */
  gl_FragColor = diffuse_intensity * v_color;
};


