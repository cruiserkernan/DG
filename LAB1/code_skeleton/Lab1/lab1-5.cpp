#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdlib>  
#include <iostream>
#include "readfile.hpp"
#include "Shaders.h"

Shaders myShaders("../lab1-5_vs.glsl", "../lab1-5_fs.glsl");

static void error_callback(int error, const char* description)
{
    std::cerr << description;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
	if ((key == GLFW_KEY_R) && action == GLFW_PRESS)
	{
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
		// Reload shaders
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
		myShaders.experimental_reload();
	}
}
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char const *argv[])
{
  // start GL context and O/S window using the GLFW helper library
  
  glfwSetErrorCallback(error_callback);
  if( !glfwInit() )
    exit(EXIT_FAILURE);
  
  GLFWwindow* window = glfwCreateWindow (640, 480, "Hello Icosahedron", NULL, NULL);
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
  if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
	}
  glfwMakeContextCurrent (window);
                                  
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit ();

//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// Set up geometry, VBO, EBO, VAO
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

  float t = (1.0f + sqrtf(5.0f))*0.25f;
  float points[] = {
	  // An icosahedron has 12 vertices
	  -0.5, t, 0,
	  0.5, t, 0,
	  -0.5, -t, 0,
	  0.5, -t, 0,
	  0, -0.5, t,
	  0, 0.5, t,
	  0, -0.5, -t,
	  0, 0.5, -t,
	  t, 0, -0.5,
	  t, 0, 0.5,
	  -t, 0, -0.5,
	  -t, 0, 0.5
  };

  unsigned short faces[] = {
	  // ... and 20 triangular faces, defined by these vertex indices:
	  0, 11, 5,
	  0, 5, 1,
	  0, 1, 7,
	  0, 7, 10,
	  0, 10, 11,
	  1, 5, 9,
	  5, 11, 4,
	  11, 10, 2,
	  10, 7, 6,
	  7, 1, 8,
	  3, 9, 4,
	  3, 4, 2,
	  3, 2, 6,
	  3, 6, 8,
	  3, 8, 9,
	  4, 9, 5,
	  2, 4, 11,
	  6, 2, 10,
	  8, 6, 7,
	  9, 8, 1
  };

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
	  0,
	  3,
	  GL_FLOAT,
	  GL_FALSE,
	  0,
	  (void*)0
  );

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
  

//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// load and compile shaders  "../lab1-5_vs.glsl" and "../lab1-5_fs.glsl"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// attach and link vertex and fragment shaders into a shader program
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
  myShaders.load();

  while (!glfwWindowShouldClose (window)) 
  {

    // update other events like input handling 
    glfwPollEvents ();
    
    // clear the drawing surface
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// Issue an appropriate glDraw*() command.
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
	glDrawElements(
		GL_TRIANGLES,
		sizeof(faces) / sizeof(faces[0]),
		GL_UNSIGNED_SHORT,
		(void*)0
	);
    glfwSwapBuffers (window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

