#version 120

// varying vec3 out_color;
varying vec2 out_tex_coord;
varying vec3 out_color;

uniform sampler2D tex0;

void main() {

  gl_FragColor = texture2D(tex0, out_tex_coord) * vec4(out_color, 1.0);
}

// varying vec2 out_tex_coord;
// 
// uniform sampler2D tex0;
// uniform vec3 color = vec3(1.0,1.0,1.0);
// uniform float opacity = 1.0;
// 
// void main() {
// 
//   vec3 crgb = texture2D(tex0, vec2(out_tex_coord.x,-out_tex_coord.y)).rgb;
//   float alph = texture2D(tex0, vec2(out_tex_coord.x,-out_tex_coord.y)).a;
//   gl_FragColor = vec4(crgb*color, alph*opacity);
// };

