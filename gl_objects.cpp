// Example of:
// - More than one object (loaded from OBJ file)
// - Keyboard events (press A to start/stop animation)
// - Mouse events
// - Orthographic camera projection
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "object_3d.h"

const unsigned int SIZE = 512;

bool animationEnabled = true;
double mousePosX = 0.0;
double mousePosY = 0.0;

GLuint ShaderProgram(const std::string &vertex_shader_file, const std::string &fragment_shader_file) {
  // Create shaders
  auto vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  auto fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  auto result = GL_FALSE;
  auto info_length = 0;

  // Load shader code
  std::ifstream vertex_shader_stream(vertex_shader_file);
  std::string vertex_shader_code((std::istreambuf_iterator<char>(vertex_shader_stream)), std::istreambuf_iterator<char>());

  std::ifstream fragment_shader_stream(fragment_shader_file);
  std::string fragment_shader_code((std::istreambuf_iterator<char>(fragment_shader_stream)), std::istreambuf_iterator<char>());

  // Compile vertex shader
  std::cout << "Compiling Vertex Shader ..." << std::endl;
  auto vertex_shader_code_ptr = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &vertex_shader_code_ptr, NULL);
  glCompileShader(vertex_shader_id);

  // Check vertex shader log
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string vertex_shader_log((unsigned int)info_length, ' ');
    glGetShaderInfoLog(vertex_shader_id, info_length, NULL, &vertex_shader_log[0]);
    std::cout << vertex_shader_log << std::endl;
  }

  // Compile fragment shader
  std::cout << "Compiling Fragment Shader ..." << std::endl;
  auto fragment_shader_code_ptr = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_shader_code_ptr, NULL);
  glCompileShader(fragment_shader_id);

  // Check fragment shader log
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string fragment_shader_log((unsigned long)info_length, ' ');
    glGetShaderInfoLog(fragment_shader_id, info_length, NULL, &fragment_shader_log[0]);
    std::cout << fragment_shader_log << std::endl;
  }

  // Create and link the program
  std::cout << "Linking Shader Program ..." << std::endl;
  auto program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glBindFragDataLocation(program_id, 0, "FragmentColor");
  glLinkProgram(program_id);

  // Check program log
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string program_log((unsigned long)info_length, ' ');
    glGetProgramInfoLog(program_id, info_length, NULL, &program_log[0]);
    std::cout << program_log << std::endl;
  }
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return program_id;
}

void UpdateProjection(GLuint program_id, bool is_perspective, glm::mat4 camera) {
  glUseProgram(program_id);

  // Create projection matrix
  glm::mat4 Projection;
  if (is_perspective) {
  // Perspective projection matrix (field of view, aspect ratio, near plane distance, far plane distance)
    Projection = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
  } else {
  // Orthographic projection matrix (left, right, bottom, top, near plane distance, far plane distance)
    Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1000.0f, 1000.0f);
  }

  // Send projection matrix value to program
  auto projection_uniform = glGetUniformLocation(program_id, "ProjectionMatrix");
  glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(Projection));

  // Send view matrix value to program
  auto view_uniform = glGetUniformLocation(program_id, "ViewMatrix");
  glm::mat4 View = glm::inverse(camera);
  glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(View));
}

void InitializeGLState() {
  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable polygon culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
}

// Keyboard press event handler
void OnKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    animationEnabled = !animationEnabled;
  }
}

// Mouse move event handler
void OnMouseMove(GLFWwindow* window, double xpos, double ypos) {
  mousePosX = (xpos / ((double) SIZE) * 2) - 1;
  mousePosY = -((ypos / ((double) SIZE) * 2) - 1);
}

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    return EXIT_FAILURE;
  }

  // Setup OpenGL context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Try to create a window
  auto window = glfwCreateWindow(SIZE, SIZE, "OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Finalize window setup
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_3")) {
    std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Add keyboard and mouse handlers
  glfwSetKeyCallback(window, OnKeyPress);
  glfwSetCursorPosCallback(window, OnMouseMove);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide mouse cursor

  // Load shaders
  GLint program_id = ShaderProgram("gl_objects.vert", "gl_objects.frag");

  // Initialize OpenGL state
  InitializeGLState();

  // Create objects from OBJ files with the specified textures
  Object3D object0 = Object3D(
    program_id, // Render object with this program (ID)
    "sphere.obj", // OBJ file
    "sphere.rgb", 256, 256 // Texture file (and its width, height)
  );
  Object3D object1 = Object3D(program_id, "sphere.obj", "sphere.rgb", 256, 256);
  Object3D cursor = Object3D(program_id, "cursor.obj", "lena.rgb", 512, 512);

  float time = 0;
  float prevTime = 0;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (animationEnabled) time += (float) glfwGetTime() - prevTime;
    prevTime = (float) glfwGetTime();

    // Camera position/rotation - for example, translate camera a bit backwards (positive value in Z axis), so we can see the objects
    glm::mat4 cameraMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));

    // --- Draw objects using perspective projection (spheres) ---
    // Create object matrices
    glm::mat4 object0ModelMat = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 object1ModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(time), cos(time), 0));
    object1ModelMat = glm::scale(object1ModelMat, glm::vec3(0.5f, 0.5f, 0.5f));

    // Update camera with perspective projection
    UpdateProjection(program_id, true, cameraMat);

    // Render objects
    object0.render(object0ModelMat);
    object1.render(object1ModelMat);

    // --- Draw objects using orthographic projection (mouse "cursor") ---
    // Create object matrix
    glm::mat4 cursorModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(mousePosX, mousePosY, 0.0f));

    // Update camera with orthographic projection
    UpdateProjection(program_id, false, cameraMat);

    // Render objects
    cursor.render(cursorModelMat);

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
