// Example gl_projection
// - Demonstrates the use of perspective projection
// - Animates object rotation
// - Useful for demonstrating culling and depth test concepts

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "mesh.h"

#include "gl_projection_vert.h"
#include "gl_projection_frag.h"

const unsigned int SIZE = 512;

#define PI 3.14159265358979323846f

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
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_projection", nullptr, nullptr);
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

  // Set up OpenGL options
  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable polygon culling
//  glEnable(GL_CULL_FACE);
//  glFrontFace(GL_CCW);
//  glCullFace(GL_BACK);

  // Load shader program
  auto program = ShaderPtr(new Shader{gl_projection_vert, gl_projection_frag});
  program->Use();

  // Load quad with Lena as texture
  auto quad = Mesh{program, "quad.obj"};
  auto texture = TexturePtr(new Texture{"lena.rgb", 512, 512});
  program->SetTexture(texture, "Texture");

  // Initialize projection
  // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
  auto projection = glm::perspective( (PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);
  program->SetMatrix(projection, "ProjectionMatrix");

  // Create view matrix (translate camera a bit backwards, so we can see the geometry)
  auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  program->SetMatrix(view, "ViewMatrix");

  // Quad positions
  auto quad1Mat = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,1));
  auto quad2Mat = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-1));

  // Time counter
  float time;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Update time and create a rotation matrix
    time = (float)glfwGetTime();
    auto rotateMat = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0,1,0));

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw two quads
    program->SetMatrix(rotateMat * quad1Mat, "ModelMatrix");
    quad.Render();

    program->SetMatrix(rotateMat * quad2Mat, "ModelMatrix");
    quad.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
