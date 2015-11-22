// Example gl_transform
// - This example demonstrates basic 2D transformations
// - Notice that the glm library uses column major order when declaring matrices
// - Try to combilne multiple transformation using multiplication

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "mesh.h"
#include "gl_transform_vert.h"
#include "gl_transform_frag.h"

const unsigned int SIZE = 512;

void SetTransformation(ShaderPtr program, float time) {
  // Create transformation matrix
  // NOTE: glm matrices are declared column major !

  // identity
  auto transform = glm::mat3({
     1.0, 0.0, 0.0,
     0.0, 1.0, 0.0,
     0.0, 0.0, 1.0
  });

  // scale
  // transform *= glm::mat3({
  //    std::sin(time), 0.0, 0.0,
  //    0.0, std::sin(time), 0.0,
  //    0.0, 0.0, 1.0
  //  });

  // squash
  // transform *= glm::mat3({
  //   std::sin(time), 0.0, 0.0,
  //   0.0, std::cos(time), 0.0,
  //   0.0, 0.0, 1.0
  // });

  // rotate
  transform *= glm::mat3({
     std::cos(time), std::sin(time), 0.0,
     -std::sin(time), std::cos(time), 0.0,
     0.0, 0.0, 1.0
  });

  // translate
  // transform *= glm::mat3({
  //    1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   std::sin(time)/2.0, std::cos(time)/2.0, 1.0
  // });

  // Rotate around top right corner
  // transform *= glm::mat3({
  //   // Move to origin
  //   1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   1.0, 1.0, 1.0
  // }) * glm::mat3({
  //   // Rotate
  //   std::cos(time), std::sin(time), 0.0,
  //   -std::sin(time), std::cos(time), 0.0,
  //   0.0, 0.0, 1.0
  // }) * glm::mat3({
  //   // Move back
  //   1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   -1.0, -1.0, 1.0
  // });

  // Send matrix value to program
  auto transform_uniform = program->GetUniformLocation("Transform");
  glUniformMatrix3fv(transform_uniform, 1, GL_FALSE, glm::value_ptr(transform));
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
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_transform", nullptr, nullptr);
  if (!window) {
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
  auto program = ShaderPtr(new Shader{gl_transform_vert, gl_transform_frag});
  program->Use();

  auto texture = Texture{"lena.rgb", 512, 512};
  auto quad = Mesh{program, "quad.obj"};

  float time = 0;
  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetTransformation(program, time+=0.01f);

    program->SetMatrix(glm::mat4(1.0f), "ModelView");
    quad.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
