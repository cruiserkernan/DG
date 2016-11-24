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
#include "lodepng.h"


#define  PI 3.14159265
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// You can store the rotation angles here, for example
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
float g_rotation[2] = { 0, 0 };
float g_translate[2] = {0, 0 };

Shaders myShaders("../lab3-3_vs.glsl", "../lab3-3_fs.glsl");

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

void calculateTangents(std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, glm::vec2& uv1, glm::vec2& uv2, glm::vec2& uv3, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3)
{
	glm::vec3 e1 = p2 - p1;
	glm::vec3 e2 = p3 - p1;
	float delta_u1 = uv2.x - uv1.x;
	float delta_v1 = uv2.y - uv1.y;

	float delta_u2 = uv3.x - uv1.x;
	float delta_v2 = uv3.y - uv1.y;

	float r = 1.0f / (delta_u1*delta_v2 - delta_v1*delta_u2);
	glm::vec3 tangent_vec = (delta_v2*e1 - delta_v1*e2) / r;
	glm::vec3 bitangent_vec = (delta_u1*e2 - delta_u2*e1) / r;



	//Push to arrays
	tangents.push_back(tangent_vec);
	tangents.push_back(tangent_vec);
	tangents.push_back(tangent_vec);

	bitangents.push_back(bitangent_vec);
	bitangents.push_back(bitangent_vec);
	bitangents.push_back(bitangent_vec);

}

void calculateAllNormals(unsigned short faces[], const int faces_count, float points[], const int normalOffset, float tangents[], float bitangents[], float uv_cords[])
{
	const int stride = 3;
	const int uvstride = 2;
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

		
		// Normal map magic


		glm::vec2 uv1;
		glm::vec2 uv2;
		glm::vec2 uv3;

		uv1.x = uv_cords[faces[i] * uvstride + 0];
		uv1.y = uv_cords[faces[i] * uvstride + 1];

		uv2.x = uv_cords[faces[i + 1] * uvstride + 0];
		uv2.y = uv_cords[faces[i + 1] * uvstride + 1];

		uv3.x = uv_cords[faces[i + 2] * uvstride + 0];
		uv3.y = uv_cords[faces[i + 2] * uvstride + 1];

		glm::vec3 e1 = p2 - p1;
		glm::vec3 e2 = p3 - p1;
		float delta_u1 = uv2.x - uv1.x;
		float delta_v1 = uv2.y - uv1.y;

		float delta_u2 = uv3.x - uv1.x;
		float delta_v2 = uv3.y - uv1.y;

		float r = 1.0f / (delta_u1*delta_v2 - delta_v1*delta_u2);
		glm::vec3 tangent_vec = (delta_v2*e1 - delta_v1*e2) * r;
		glm::vec3 bitangent_vec = (delta_u1*e2 - delta_u2*e1) * r;


		tangents[faces[i] * stride + 0] = tangent_vec.x;
		tangents[faces[i] * stride + 1] = tangent_vec.y;
		tangents[faces[i] * stride + 2] = tangent_vec.z;

		tangents[faces[i + 1] * stride + 0] = tangent_vec.x;
		tangents[faces[i + 1] * stride + 1] = tangent_vec.y;
		tangents[faces[i + 1] * stride + 2] = tangent_vec.z;

		tangents[faces[i + 2] * stride + 0] = tangent_vec.x;
		tangents[faces[i + 2] * stride + 1] = tangent_vec.y;
		tangents[faces[i + 2] * stride + 2] = tangent_vec.z;

		bitangents[faces[i] * stride + 0] = bitangent_vec.x;
		bitangents[faces[i] * stride + 1] = bitangent_vec.y;
		bitangents[faces[i] * stride + 2] = bitangent_vec.z;

		bitangents[faces[i + 1] * stride + 0] = bitangent_vec.x;
		bitangents[faces[i + 1] * stride + 1] = bitangent_vec.y;
		bitangents[faces[i + 1] * stride + 2] = bitangent_vec.z;

		bitangents[faces[i + 2] * stride + 0] = bitangent_vec.x;
		bitangents[faces[i + 2] * stride + 1] = bitangent_vec.y;
		bitangents[faces[i + 2] * stride + 2] = bitangent_vec.z;



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
	if ((key == GLFW_KEY_W) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_translate[0] += 0.10;
	}
	if ((key == GLFW_KEY_S) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_translate[0] -= 0.10;
	}
	if ((key == GLFW_KEY_A) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_translate[1] += 0.10;
	}
	if ((key == GLFW_KEY_D) && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		g_translate[1] -= 0.10;
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


	int num_faces = 20;
	int num_verts = 60;

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

	float uv_coords[] =
	{
		0.072405, 0.144686,
		0.29888,  0.70651,
		0.576437, 0.721299,
		0.149690, 0.122508,
		0.33436,  0.68882,
		0.103319, 0.713906,
		0.772001, 0.728691,
		0.520657, 0.721299,
		0.914475, 0.743476,
		0.140953, 0.728691,
		0.723614, 0.728691,
		0.045523, 0.691729,

		0.27133,  0.10033,
		0.449421, 0.736084,
		0.319044, 0.750869,
		0.347942, 0.115116,
		0.181276, 0.721299,
		0.24512,  0.70651,
		0.618776, 0.721299,
		0.650362, 0.736084,
		0.819717, 0.736084,
		0.122136, 0.706514,
		0.897001, 0.721299,
		0.967566, 0.721299,

		0.481007, 0.129901,
		0.108695, 0.129901,
		0.548883, 0.736084,
		0.583157, 0.107723,
		0.379528, 0.721299,
		0.083157, 0.699121,
		0.745120, 0.721299,
		0.700765, 0.728691,
		0.201437, 0.721299,
		0.262593, 0.721299,
		0.499152, 0.736084,
		0.28341,  0.67157,

		0.681948, 0.115116,
		0.847270, 0.699121,
		0.388265, 0.115116,
		0.780738, 0.100331,
		0.190684, 0.122508,
		0.31006,  0.10251,
		0.433292, 0.115116,
		0.878184, 0.107723,
		0.633561, 0.122508,
		0.157754, 0.728691,
		0.731007, 0.115116,
		0.532754, 0.122508,

		0.928587, 0.107723,
		0.868776, 0.728691,
		0.397674, 0.713906,
		0.229663, 0.107723,
		0.358023, 0.736084,
		0.222942, 0.721299,
		0.598614, 0.713906,
		0.671867, 0.743476,
		0.796195, 0.743476,
		0.825765, 0.115116,
		0.469582, 0.721299,
		0.418507, 0.706514,
	};

	int num_indices = num_faces * 3;

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
	float tangents[180], bitangents[180];//changed this
	calculateAllNormals(faces, (sizeof(faces) / sizeof(faces[0])), points, (sizeof(points) / sizeof(points[0])) / 2, tangents, bitangents, uv_coords);




	// Populate your VBO with shapes[0].mesh.positions and shapes[0].mesh.normals
	// ...
	// Populate your EBO with shapes[0].mesh.indices
	// ...);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[4];
	glGenBuffers(4, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
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
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_coords), uv_coords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);//Something with this!
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents), bitangents, GL_STATIC_DRAW); //Something with this!
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4,
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

	//Create textures

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);


	//Load images
	unsigned char* image_data;
	unsigned image_w;
	unsigned image_h;
	unsigned image_file =
		lodepng_decode32_file(&image_data, &image_w, &image_h,
			"../../common/data/numberline_hires.png");
	printf("Image_file: %d\n", image_file);
	std::cout << "Die texture: Read width " << image_w << " x height " << image_h << " image\n";


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glGenerateMipmap(GL_TEXTURE_2D);

	//Textur för cube map
	GLuint texture_cube_handle;
	glGenTextures(1, &texture_cube_handle);
	glActiveTexture(GL_TEXTURE1); //Antar att det blir 1 nu ?
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cube_handle);

	//Load images (one for each side!)
	unsigned char* cube_image_data[6];


	image_file = lodepng_decode32_file(&cube_image_data[0], &image_w, &image_h, "../../common/data/cube-room_right.png");
	std::cout << "Cube map2: Read width " << image_w << " x height " << image_h << " image\n";
	image_file = lodepng_decode32_file(&cube_image_data[1], &image_w, &image_h, "../../common/data/cube-room_left.png");
	std::cout << "Cube map4: Read width " << image_w << " x height " << image_h << " image\n";
	image_file = lodepng_decode32_file(&cube_image_data[2], &image_w, &image_h, "../../common/data/cube-room_top.png");
	std::cout << "Cube map3: Read width " << image_w << " x height " << image_h << " image\n";
	image_file = lodepng_decode32_file(&cube_image_data[3], &image_w, &image_h, "../../common/data/cube-room_bottom.png");
	std::cout << "Cube map5: Read width " << image_w << " x height " << image_h << " image\n";
	image_file = lodepng_decode32_file(&cube_image_data[4], &image_w, &image_h, "../../common/data/cube-room_back.png");
	std::cout << "Cube map6: Read width " << image_w << " x height " << image_h << " image\n";
	image_file = lodepng_decode32_file(&cube_image_data[5], &image_w, &image_h, "../../common/data/cube-room_front.png");
	std::cout << "Cube map7: Read width " << image_w << " x height " << image_h << " image\n";

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, cube_image_data[i]);

	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//Bump map
	GLuint texture_normal_map_handle;
	glGenTextures(1, &texture_normal_map_handle);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_normal_map_handle);
	unsigned char* nmap_data;
	image_file = lodepng_decode32_file(&nmap_data, &image_w, &image_h, "../../common/data/numberline_nmap_hires.png");
	std::cout << "Nmap hi res: Read width " << image_w << " x height " << image_h << " image\n";

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nmap_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

									 //-----------------------------------------------------------------------------------------------------------------------------------------------------------//
									 // attach and link vertex and fragment shaders into a shader program
									 //-----------------------------------------------------------------------------------------------------------------------------------------------------------//
	myShaders.load();
	checkShaderCompileError(myShaders.getVS());
	checkShaderCompileError(myShaders.getFS());
	float n = 0.01;
	float f = 100.0;

	glm::vec3 light1_colour(1, 1, 1);
	glm::vec3 light1_position(1000, 0, 0);
	glm::vec3 light2_colour(1, 1, 1);
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


		auto projectionMatrix = glm::perspective(45.0f, (float(w_width) / w_height), n, f);

		auto rotate_y = glm::rotate(g_rotation[1], glm::vec3(0, 1, 0));
		auto rotate_x = glm::rotate(g_rotation[0], glm::vec3(1, 0, 0));
		auto scale = glm::scale(glm::vec3(1));
		auto translate = glm::translate(glm::vec3(g_translate[1], 0, g_translate[0]));
		auto modelMatrix = translate * rotate_x * rotate_y * scale;

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

		//Texture uniform
		glUniform1i(glGetUniformLocation(myShaders.get_shader_program(), "tex_sampler"), 0);
		glUniform1i(glGetUniformLocation(myShaders.get_shader_program(), "cube_sampler"), 1);
		glUniform1i(glGetUniformLocation(myShaders.get_shader_program(), "normal_sampler"), 2);

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

