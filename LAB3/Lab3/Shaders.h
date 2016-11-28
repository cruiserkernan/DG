#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "readfile.hpp"
#include <vector>

class Shaders
{
	private:
		std::string vertex_shader_str;
		std::string fragment_shader_str;
		std::string vertex_filename;
		std::string fragment_filename;
		std::string tesselation_control_shader_str;
		std::string tesselation_evaluation_shader_str;
		std::string tesselation_control_filename;
		std::string tesselation_evaluation_filename;
		GLuint shader_program;
		GLuint vs;
		GLuint fs;
		GLuint tes;
		GLuint tcs;
		GLint offset_location = 0;
		GLint modifier_location = 0;

		void update_locations()
		{
			offset_location = glGetUniformLocation(get_shader_program(), "position_offset");
			modifier_location = glGetUniformLocation(get_shader_program(), "modifier");
		}
	public:
		Shaders(){}
		Shaders(std::string vertex_filename, std::string fragment_filename, std::string tesselation_control_filename, std::string tesselation_evaluation_filename)
		{
			this->vertex_filename = vertex_filename;
			this->fragment_filename = fragment_filename;
			this->tesselation_control_filename = tesselation_control_filename;
			this->tesselation_evaluation_filename = tesselation_evaluation_filename;
		}
		GLint get_offset_location() { return offset_location; }
		GLint get_modifier_location() { return modifier_location; }

		void load()
		{
			vertex_shader_str = readFile(vertex_filename.c_str());
			fragment_shader_str = readFile(fragment_filename.c_str());
			const char *vertex_shader_src = vertex_shader_str.c_str();
			const char *fragment_shader_src = fragment_shader_str.c_str();
			vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertex_shader_src, NULL);
			glCompileShader(vs);
			fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fragment_shader_src, NULL);
			glCompileShader(fs);
			
			


			tesselation_control_shader_str = readFile(tesselation_control_filename.c_str());
			tesselation_evaluation_shader_str = readFile(tesselation_evaluation_filename.c_str());
			const char *tesselation_control_shader_src = tesselation_control_shader_str.c_str();
			const char *tesselation_evaluation_shader_src = tesselation_evaluation_shader_str.c_str();
			tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tcs, 1, &tesselation_control_shader_src, NULL);
			glCompileShader(tcs);
			tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(tes, 1, &tesselation_evaluation_shader_src, NULL);
			glCompileShader(tes);
			
			shader_program = glCreateProgram();


			glAttachShader(shader_program, tcs);
			glAttachShader(shader_program, tes);
			glAttachShader(shader_program, fs);
			glAttachShader(shader_program, vs);
			
			glLinkProgram(shader_program);

			GLint isLinked = 0;
			glGetProgramiv(shader_program, GL_LINK_STATUS, (int *)&isLinked);

			if (isLinked == GL_FALSE)
			{
				std::cout << "ERROR IN LINKING!!!!" << std::endl;
				GLint maxLength = 0;
				glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &maxLength);

				//The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(shader_program, maxLength, &maxLength, &infoLog[0]);

				std::cerr << infoLog.data() << std::endl;

			}
			else { std::cout << "Linking sucessfull..." << std::endl; }

			glUseProgram(shader_program);

			//Delete shaders
			glDeleteShader(vs);
			glDeleteShader(fs);
			glDeleteShader(tcs);
			glDeleteShader(tes);

			update_locations();
		}

		void experimental_reload()
		{
			glDetachShader(shader_program, vs);
			glDetachShader(shader_program, fs);
			glDetachShader(shader_program, tes);
			glDetachShader(shader_program, tcs);
			vertex_shader_str = readFile(vertex_filename.c_str());
			fragment_shader_str = readFile(fragment_filename.c_str());
			const char *vertex_shader_src = vertex_shader_str.c_str();
			const char *fragment_shader_src = fragment_shader_str.c_str();
			vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertex_shader_src, NULL);
			glCompileShader(vs);
			fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fragment_shader_src, NULL);
			glCompileShader(fs);
			glAttachShader(shader_program, fs);
			glAttachShader(shader_program, vs);


			tesselation_control_shader_str = readFile(tesselation_control_filename.c_str());
			tesselation_evaluation_shader_str = readFile(tesselation_evaluation_filename.c_str());
			const char *tesselation_control_shader_src = tesselation_control_shader_str.c_str();
			const char *tesselation_evaluation_shader_src = tesselation_evaluation_shader_str.c_str();
			vs = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tcs, 1, &tesselation_control_shader_src, NULL);
			glCompileShader(tcs);
			fs = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(tes, 1, &tesselation_evaluation_shader_src, NULL);
			glCompileShader(tes);
			glAttachShader(shader_program, tcs);
			glAttachShader(shader_program, tes);

			

			glLinkProgram(shader_program);

			
			//Delete shaders
			glDeleteShader(vs);
			glDeleteShader(fs);
			glDeleteShader(tcs);
			glDeleteShader(tes);

		}

		GLuint get_shader_program()
		{
			return shader_program;
		}

		void reload()
		{
			glDeleteProgram(shader_program);
			load();
		}

		GLint getVS() { return vs; }
		GLint getFS() { return fs; }
		GLint getTES() { return tes; }
		GLint getTCS() { return tcs; }
	};

