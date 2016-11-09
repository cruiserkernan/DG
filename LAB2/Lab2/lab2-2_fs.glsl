#version 440

out vec4 frag_colour;
in vec4 normal;
in vec4 position;

uniform int light_count;
uniform vec4 light_position[4]; // can have up to 4 light sources
uniform vec4 light_colour[4];

vec4 camera_position = {0, 0, 1, 1};

vec4 lambertian_brdf( vec4 in_direction, vec4 out_direction, vec4 normal )
{
  // YOUR CODE GOES HERE
  // Implement a Lambertian BRDF (that does not only return one).
  return 1;
}

void main () {

  frag_colour = vec4(0);
  for (int l = 0; l < light_count; ++l )
  {
    // YOUR CODE GOES HERE
    // Implement Equation 3 from the lab instructions.      
  }
}
