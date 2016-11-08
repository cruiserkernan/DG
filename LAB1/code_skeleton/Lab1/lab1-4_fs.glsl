#version 400
out vec4 frag_colour;
in vec3 position;
uniform float modifier;
void main () {
  frag_colour = vec4 (position, 1.0) + vec4(0, 0, modifier, 0) ;
}
