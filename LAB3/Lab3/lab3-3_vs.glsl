#version 400

layout(location=0) in vec4 vp;
layout(location=1) in vec4 n;
layout(location=2) in vec2 uv_in;
layout(location=3) in vec4 tangent_in;
layout(location=4) in vec4 bitangent_in;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
out vec4 position;
out vec4 normal;
out vec2 uv;
out vec4 tangent;
out vec4 bitangent;
void main () {
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// Apply the model, view and projection transform to vertex positions and forward the position to the fragment shader using an appropriate "out" variable
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
  position = modelViewMatrix * vp;
  normal = normalMatrix * n;
  uv = uv_in;
  tangent = normalMatrix * tangent_in;
  bitangent = normalMatrix * bitangent_in;
  gl_Position = modelViewProjectionMatrix * vp;


};
  