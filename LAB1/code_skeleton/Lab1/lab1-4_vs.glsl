#version 400
in vec3 vp;
out vec3 position;
uniform vec2 position_offset;

void main () {
  gl_Position = vec4 (vp, 1.0) + vec4(position_offset, 0, 0);
  position = (vp + vec3(1,1,1)) / 2;	
 };
  