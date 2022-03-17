#version 120

varying vec3 frag_pos;

uniform samplerCube skybox;

void main() {


  gl_FragColor = textureCube(skybox, frag_pos);
};


