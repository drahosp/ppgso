// Example gl_gradient
// - Demonstrates the use of a dynamically generated gradient in GLSL
// - Displays the gradient on a quad using OpenGL

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "gl_gradient_vert.h"
#include "gl_gradient_frag.h"

const unsigned int SIZE = 512;

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
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_gradient", NULL, NULL);
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

  // Load shaders
  auto program = ShaderPtr(new Shader{gl_gradient_vert, gl_gradient_frag});
  program->Use();

  // Generate a vertex array object
  // This keeps track of what attributes are associated with buffers
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Setup geometry
  std::vector<GLfloat> vertex_buffer{
          // x, y
          1.0f, 1.0f,
          -1.0f, 1.0f,
          1.0f, -1.0f,
          -1.0f, -1.0f
  };

  // Generate a vertex buffer object, this will feed data to the vertex shader
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

  // Setup vertex array lookup, this tells the shader how to pick data for the "Position" input
  auto position_attrib = program->GetAttribLocation("Position");
  glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attrib);

  // Colors for vertices
  std::vector<GLfloat> color_buffer{
          // r, g, b
          1.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f,
          0.0, 0.0f, 1.0f,
          1.0f, 1.0f, 1.0f
  };

  GLuint cbo;
  glGenBuffers(1, &cbo);
  glBindBuffer(GL_ARRAY_BUFFER, cbo);
  glBufferData(GL_ARRAY_BUFFER, color_buffer.size() * sizeof(GLfloat), color_buffer.data(), GL_STATIC_DRAW);

  // Same thing for colors
  auto color_attrib = program->GetAttribLocation("Color");
  glVertexAttribPointer(color_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(color_attrib);

  // Main execution loop
  while (!glfwWindowShouldClose(window) ) {
    // Set gray background
    glClearColor(.5,.5,.5,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glfwTerminate();

  return EXIT_SUCCESS;
}
