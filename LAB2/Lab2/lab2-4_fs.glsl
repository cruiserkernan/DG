#version 440

out vec4 frag_colour;
in vec4 normal;
in vec4 position;

uniform int light_count;
uniform vec3 light_position[4]; // can have up to 4 light sources
uniform vec3 light_colour[4];

vec4 camera_position = {0, 0, 1, 1};

vec3 lambertian_brdf( vec3 in_direction, vec3 out_direction, vec3 normal )
{  
  return vec3(1/3.14);
}

void main () {

	vec3 sum = vec3(0);
	vec3 normalized_normal = normalize(normal.xyz);
	vec3 Wi;
	vec3 Wr;
	vec3 W0;
	for (int i = 0; i < light_count; ++i )
	{
		Wi = normalize(light_position[i] - position.xyz);
		W0 = normalize(vec3(0,0,0) - position.xyz); //0,0,0 since it is in camera space
		sum += lambertian_brdf(Wi, W0, normalized_normal) * light_colour[i] * max(dot(Wi, normalized_normal), 0);    
	}

  frag_colour = vec4(sum, 0);
}
