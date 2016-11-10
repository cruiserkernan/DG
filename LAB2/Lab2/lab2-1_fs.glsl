#version 440

out vec4 frag_colour;
in vec4 normal;
in vec4 position;

uniform int light_count;
uniform vec3 light_position[4]; // can have up to 4 light sources
uniform vec3 light_colour[4];

//vec4 camera_position = {0, 0, -2, 0}; Not needed here, confused us !

void main () {

	vec3 Ca = vec3(0.1, 0.1, 0.1);
	vec3 Cd = vec3(0.8, 0.8, 0.5);
	vec3 Cs = vec3(1.0, 1.0, 1.0);
	float f = 20; 				  
	vec3 Ia = vec3(0.2, 0.2, 0.2);
	vec3 Id = vec3(0.8, 0.8, 0.8);
	vec3 Is = vec3(1.0, 1.0, 1.0);
	vec3 sum = vec3(0);
	vec3 localnormal = (normalize(normal.xyz));
	vec3 Wi;
	vec3 Wr;
	vec3 v;


	for (int i = 0; i < light_count; ++i )
	{
		Wi = normalize(light_position[i] - position.xyz);
		Wr = normalize(reflect(-Wi, localnormal));	//Minus Wi since Wi is originally directed towards light
		v = normalize(vec3(0,0,0) - position.xyz); //0,0,0 since it is in camera space

		//Diffuse
		vec3 diffuse_term = light_colour[i] * Cd * Id * (max(dot(localnormal,Wi),0));
		diffuse_term = clamp(diffuse_term,0,1);

		//Specular
		vec3 specular_term = light_colour[i] * Cs * Is * pow(max(dot(Wr,v),0),f);
		specular_term = clamp(specular_term,0,1);

		sum += vec3(diffuse_term + specular_term);
	}

	frag_colour = vec4(Ca * Ia + sum, 1);
}

