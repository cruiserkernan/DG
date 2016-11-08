#version 400

layout(location=0) in vec4 vp;
uniform mat4 modelViewProjectionMatrix;
out vec4 position;

void main () {
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// Apply the model, view and projection transform to vertex positions and forward the position to the fragment shader using an appropriate "out" variable
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
  position = modelViewProjectionMatrix * vp;
  gl_Position = modelViewProjectionMatrix * vp;

};
  