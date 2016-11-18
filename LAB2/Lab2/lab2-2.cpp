// GL Extension Wrangler
#include <GL/glew.h>
#include <glm.hpp>
#include <gtx/projection.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include <cmath>
// http://www.glfw.org/faq.html#what-is-glfw
//OpenGL doesn't have a way of creating window, this does it in an OS-independent way
#include <GLFW/glfw3.h>
#include <cstdlib>  
#include <iostream>
#include "readfile.hpp"
#include "Shaders.h"

#define  PI 3.14159265
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// You can store the rotation angles here, for example
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
float g_rotation[2] = { 0, 0 };

Shaders myShaders("../lab2-2_vs.glsl", "../lab2-2_fs.glsl");

void checkShaderCompileError(GLint shaderID)
{
	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::string errorLog;
		errorLog.resize(maxLength);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

		std::cout << "shader compilation failed:" << std::endl;
		std::cout << errorLog << std::endl;
		return;
	}
	else
		std::cout << "shader compilation success." << std::endl;

	return;
}

#define OFFSET 12
void hackerfunction(unsigned short faces[])
{
	int record_array[12] = { 0 };

	for (int i = 0; i < 60; i++)
	{
		if (record_array[faces[i]] > 0) {
			auto index = faces[i];
			faces[i] = faces[i] + (record_array[index] * OFFSET);
			record_array[index]++;
		}
		else
		{
			//Increase record array value + 1
			record_array[faces[i]]++;
		}
	}
}
glm::vec3 calculateSurfaceNormal(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3)
{
	glm::vec3 U = p2 - p1;
	glm::vec3 V = p3 - p1;
	return glm::cross(U, V);
}
void calculateAllNormals(unsigned short faces[], const int faces_count, float points[], const int normalOffset)
{
	const int stride = 3;
	for (size_t i = 0; i < faces_count; i += stride)
	{
		glm::vec3 p1, p2, p3;
		p1.x = points[faces[i] * stride];
		p1.y = points[faces[i] * stride + 1];
		p1.z = points[faces[i] * stride + 2];

		p2.x = points[faces[i + 1] * stride];
		p2.y = points[faces[i + 1] * stride + 1];
		p2.z = points[faces[i + 1] * stride + 2];

		p3.x = points[faces[i + 2] * stride];
		p3.y = points[faces[i + 2] * stride + 1];
		p3.z = points[faces[i + 2] * stride + 2];

		auto surfaceNormal = calculateSurfaceNormal(p1, p2, p3);

		points[faces[i] * stride + normalOffset] = surfaceNormal.x;
		points[faces[i] * stride + normalOffset + 1] = surfaceNormal.y;
		points[faces[i] * stride + normalOffset + 2] = surfaceNormal.z;

		points[faces[i + 1] * stride + normalOffset] = surfaceNormal.x;
		points[faces[i + 1] * stride + normalOffset + 1] = surfaceNormal.y;
		points[faces[i + 1] * stride + normalOffset + 2] = surfaceNormal.z;

		points[faces[i + 2] * stride + normalOffset] = surfaceNormal.x;
		points[faces[i + 2] * stride + normalOffset + 1] = surfaceNormal.y;
		points[faces[i + 2] * stride + normalOffset + 2] = surfaceNormal.z;
	}

}


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

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
	// Update rotation angle here, for example
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

	if ((key == GLFW_KEY_RIGHT) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_rotation[1] += 0.10;
	}
	if ((key == GLFW_KEY_LEFT) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_rotation[1] -= 0.10;
	}
	if ((key == GLFW_KEY_UP) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_rotation[0] += 0.10;
	}
	if ((key == GLFW_KEY_DOWN) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_rotation[0] -= 0.10;
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
	if (!glfwInit())
		exit(EXIT_FAILURE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Icosahedron", NULL, NULL);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	int w_height = 800;
	int w_width = 800;

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

						  //-----------------------------------------------------------------------------------------------------------------------------------------------------------//
						  // Set up geometry, VBO, EBO, VAO
						  //-----------------------------------------------------------------------------------------------------------------------------------------------------------//
	float t = (1.0f + sqrtf(5.0f))*0.25f;
	float points[360] = {
		// An icosahedron has 12 vertices, each vertex shared by five faces
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
		-t, 0, 0.5,

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
		-t, 0, 0.5,

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
		-t, 0, 0.5,

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
		-t, 0, 0.5,

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

	hackerfunction(faces);

	calculateAllNormals(faces, (sizeof(faces) / sizeof(faces[0])), points, (sizeof(points) / sizeof(points[0])) / 2);

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
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(sizeof(points) / 2)
	);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
	// attach and link vertex and fragment shaders into a shader program
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
	myShaders.load();
	checkShaderCompileError(myShaders.getVS());
	checkShaderCompileError(myShaders.getFS());
	float n = 1.0;
	float f = 100.0;

	glm::vec3 light1_colour(1, 1, 1);
	glm::vec3 light1_position(2, 1, 2);
	glm::vec3 light2_colour(0.2, 0.2, 1);
	glm::vec3 light2_position(-2, 1, 2);
	glm::vec3 light_colour[] = { light1_colour, light2_colour };
	glm::vec3 light_position[] = { light1_position, light2_position };
	int light_count = 2;

	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &w_width, &w_height); //you might need this for correcting the aspect ratio

															 //-----------------------------------------------------------------------------------------------------------------------------------------------------------//
															 // Define the projection matrix, rotation matrices, model matrix, etc. The variable names and code structure is a simple suggestion, you may improve on it!
															 //-----------------------------------------------------------------------------------------------------------------------------------------------------------//


		auto projectionMatrix = glm::perspective(90.0f, (float(w_width) / w_height), n, f);

		auto rotate_y = glm::rotate(g_rotation[1], glm::vec3(0, 1, 0));
		auto rotate_x = glm::rotate(g_rotation[0], glm::vec3(1, 0, 0));

		auto modelMatrix = rotate_x * rotate_y;

		auto inverseViewMatrix = glm::inverse(glm::translate(glm::vec3(0, 0, 2)));


		auto modelViewMatrix = inverseViewMatrix * modelMatrix;
		auto normalMatrixNotTransposedYet = glm::inverse(modelViewMatrix);
		auto modelViewProjectionMatrix = projectionMatrix * inverseViewMatrix * modelMatrix;


		//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
		// Send your modelViewProjection matrix to your vertex shader as a uniform varable
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

		glUniformMatrix4fv(glGetUniformLocation(myShaders.get_shader_program(), "modelViewProjectionMatrix"), 1, 0, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(myShaders.get_shader_program(), "modelViewMatrix"), 1, 0, glm::value_ptr(modelViewMatrix));
		//Normal matrix transposed here
		glUniformMatrix4fv(glGetUniformLocation(myShaders.get_shader_program(), "normalMatrix"), 1, 1, glm::value_ptr(normalMatrixNotTransposedYet));
		glUniform3fv(glGetUniformLocation(myShaders.get_shader_program(), "light_colour"), light_count, glm::value_ptr(light_colour[0]));
		glUniform3fv(glGetUniformLocation(myShaders.get_shader_program(), "light_position"), light_count, glm::value_ptr(light_position[0]));
		glUniform1i(glGetUniformLocation(myShaders.get_shader_program(), "light_count"), light_count);

		// update other events like input handling 
		glfwPollEvents();

		// clear the drawing surface
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
		// Issue an appropriate glDraw*() command.
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

		//glDrawArrays(GL_TRIANGLES, 0, sizeof(points) / sizeof(float));

		glDrawElements(
			GL_TRIANGLES,
			sizeof(faces) / sizeof(faces[0]),
			GL_UNSIGNED_SHORT,
			(void*)0
		);

		glfwSwapBuffers(window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

