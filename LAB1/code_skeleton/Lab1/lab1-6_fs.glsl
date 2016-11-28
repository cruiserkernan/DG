#version 400
out vec4 frag_colour;
in vec4 position;
uniform mat4 modelViewProjectionMatrix;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main () {
	vec3 z = hsv2rgb(vec3((position.z+1), 1, 1));
	frag_colour = vec4(position.zzz,1);
}
