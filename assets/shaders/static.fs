#version 120

varying vec3 norm;
varying vec2 uv;

uniform vec3 light;
uniform sampler2D tex0;

void main() {

  vec4 diffuse_color = texture2D(tex0, uv);

  // vec3 n = normalize(norm);
  // vec3 l = normalize(light);

  // float diffuse_intensity = clamp(dot(n, l), 0.0, 1.0);

  gl_FragColor = diffuse_color;
};

