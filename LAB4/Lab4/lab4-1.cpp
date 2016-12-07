#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>  
#include <iostream>

#include "readfile.hpp"

#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> //glm::value_ptr

#ifndef M_PI
#define M_PI 3.14159265359
#endif

glm::vec4 mouse_state = glm::vec4(0);
glm::vec3 position, dir, right, up = glm::vec3( 0, 0, 0 ); 

float delta_time= 0.0f;
float yaw = M_PI;
float pitch = 0;
float focal_dist = 512.0f;
float move_speed = 0.08; 
float mouse_speed = 0.001f;

int w_width = 512;
int w_height = 512;
bool first_frame = true;

void checkShaderCompileError(GLint shaderID)
{
  GLint isCompiled = 0;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
 
  if(isCompiled == GL_FALSE)
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


static void error_callback(int error, const char* description)
{
    std::cerr << description;
}

static void scroll_callback(GLFWwindow* window, double scroll_v, double scroll_h)
{
  mouse_state.x += scroll_v;
  mouse_state.y += scroll_h;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Move forward
  if ( (key == GLFW_KEY_UP   || key == GLFW_KEY_W) && 
       (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    position += dir * move_speed;
  }
  // Move backward
  if ( (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) && 
       (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    position -= dir * move_speed;
  }
  // Strafe right
  if ( (key == GLFW_KEY_RIGHT|| key == GLFW_KEY_D) && 
       (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    position += right * move_speed;
  }
  // Strafe left
  if ( (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && 
       (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    position -= right * move_speed;
  }
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS )
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void update_camera_basis()
{
  dir = glm::vec3(cos(pitch) * sin(yaw), 
                  sin(pitch),
                  cos(pitch) * cos(yaw));

  right = glm::vec3(  sin(yaw - M_PI/2.0f), 
                                0,
                                cos(yaw - M_PI/2.0f));
  up = glm::cross( right, dir );
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

  //only happens once
  static double previous_time = glfwGetTime();
  double current_time = glfwGetTime();
  delta_time = static_cast<float>(current_time - previous_time);

  // Reset mouse position
  glfwSetCursorPos(window, w_width/2.0f, w_height/2.0f);

  if(first_frame){  
    update_camera_basis();
    first_frame = false; 
    yaw = M_PI;
    pitch = 0;

    return;
  }

  // Compute new orientation
  yaw   += mouse_speed * float( w_width/2.0f - xpos );
  pitch += mouse_speed * float( w_height/2.0f - ypos );
  update_camera_basis();
    
  previous_time = current_time;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    mouse_state.z = 1;

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    mouse_state.w = 1;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    mouse_state.z = 0;

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    mouse_state.w = 0;
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
  
  GLFWwindow* window = glfwCreateWindow (w_width, w_height, "Hello Raycasting", NULL, NULL);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback ( window, scroll_callback); 
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent (window);
                                  
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit ();

  //this will be the image plane: two triangles, spanning the entire screen.
  float points[] = {
    -1, -1, 0,
    -1, +1, 0,
    +1, -1, 0,
    +1, +1, 0
  };

  position = glm::vec3( 0.5, 0.5, 2.5);

  GLuint vbo = 0;
  glGenBuffers (1, &vbo);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, 12 * sizeof (float), points, GL_STATIC_DRAW);
  
  GLuint vao = 0;
  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glEnableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  std::string vertex_shader_str = readFile("../lab4-1_vs.glsl");
  std::string fragment_shader_str = readFile("../lab4-1_fs.glsl");
  const char *vertex_shader_src = vertex_shader_str.c_str();
  const char *fragment_shader_src = fragment_shader_str.c_str();

  GLuint vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 1, &vertex_shader_src, NULL);
  glCompileShader (vs);
  checkShaderCompileError(vs);

  GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 1, &fragment_shader_src, NULL);
  glCompileShader (fs);
  checkShaderCompileError(fs);

  GLuint shader_program = glCreateProgram ();
  glAttachShader (shader_program, fs);
  glAttachShader (shader_program, vs);
  glLinkProgram (shader_program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  float start_time = glfwGetTime();

  glUseProgram (shader_program);
  glBindVertexArray (vao);
  update_camera_basis();

  while (!glfwWindowShouldClose (window)) 
  {

    GLsync s;

    glfwGetFramebufferSize(window, &w_width , &w_height );
        
    // update other events like input handling 
    glfwPollEvents ();

    glUniform1f(glGetUniformLocation(shader_program, "i_global_time"),  glfwGetTime()-start_time);
    glUniform4f(glGetUniformLocation(shader_program, "i_mouse_state"),  mouse_state.x,  mouse_state.y, mouse_state.z, mouse_state.w);
    glUniform2f(glGetUniformLocation(shader_program, "i_window_size"),  w_width, w_height);
    
    glUniform3f(glGetUniformLocation(shader_program, "i_position"),       position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shader_program, "i_up"),             up.x,  up.y, up.z);
    glUniform3f(glGetUniformLocation(shader_program, "i_dir"),            dir.x,  dir.y, dir.z);
    glUniform3f(glGetUniformLocation(shader_program, "i_right"),          right.x,  right.y, right.z);
    glUniform1f(glGetUniformLocation(shader_program, "i_focal_dist"),     focal_dist);
    glUniform3f(glGetUniformLocation(shader_program, "i_light_position"), 6e3,  1e4, 1e4);
    
    // clear the drawing surface
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    double tic = glfwGetTime();
    
    glDrawArrays(GL_TRIANGLE_STRIP,0, 4);
    
    double tic_toc = glfwGetTime() - tic;
    if(tic_toc > 1.0) break;

    glfwSwapBuffers (window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
