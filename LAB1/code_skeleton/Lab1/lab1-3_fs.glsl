#version 400
out vec4 frag_colour;
in vec3 position;

void main () {
  frag_colour = vec4 (position, 1.0);
}
