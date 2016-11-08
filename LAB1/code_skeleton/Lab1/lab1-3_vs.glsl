#version 400
in vec3 vp;
out vec3 position;

void main () {
  gl_Position = vec4 (vp, 1.0);
  position = (vp + vec3(1,1, 0)) / 2;	
 };
  