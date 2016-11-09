#version 400

layout(location=0) in vec4 vp;
layout(location=1) in vec4 n;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
out vec4 position;
out vec4 normal;

void main () {
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// Apply the model, view and projection transform to vertex positions and forward the position to the fragment shader using an appropriate "out" variable
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
  position = modelViewMatrix * vp;
  normal = modelViewMatrix * n;
  gl_Position = modelViewProjectionMatrix * vp;

};
  