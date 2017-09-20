// Example gl1_gradient
// - Demonstrates the use of a dynamically generated gradient in GLSL
// - Displays color gradient on a triangle using OpenGL

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include <shaders/color_frag_glsl.h>
#include <shaders/color_vert_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

/*!
 * Custom window that will display a gradient with OpenGL
 * Inherits from ppgso::Window which hides implementation using the glfw library
 */
class GradientWindow : public Window {
private:
  // Vertices a single triangle
  vector<vec3> vertex_buffer{
      // x, y
      {0.0f, 0.8f, 0.0f},
      {0.8f,  -0.8f, 0.0f}, 
      {-0.8f, -0.8f, 0.0f},
  };

  // Colors for vertices
  vector<vec3> color_buffer{
      // r, g, b
      {1.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
      {0.0,  0.0f, 1.0f},
  };

  // Shader program to use for rendering
  Shader program = {color_vert_glsl, color_frag_glsl};

  // OpenGL object handles to keep track of
  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint cbo = 0;
public:
  /*!
   * Window constructor that will initialize needed OpenGL resources
   */
  GradientWindow() : Window{"gl1_gradient", SIZE, SIZE} {
    // Generate a vertex array object
    // This keeps track of our data buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate a vertex buffer object, this will feed data to the vertex shader
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(vec3), vertex_buffer.data(), GL_STATIC_DRAW);

    // Setup vertex array lookup, this tells the shader how to pick data for the "Position" input
    auto position_attrib = program.getAttribLocation("Position");
    glVertexAttribPointer(position_attrib, sizeof(vec3)/sizeof(float), GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(position_attrib);

    // Same thing for colors
    glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, color_buffer.size() * sizeof(vec3), color_buffer.data(), GL_STATIC_DRAW);

    auto color_attrib = program.getAttribLocation("Color");
    glVertexAttribPointer(color_attrib, sizeof(vec3)/sizeof(float), GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(color_attrib);

    // Set Matrices to identity so there are no projections/transformations applied in the vertex shader
    program.setUniform("ModelMatrix", mat4{});
    program.setUniform("ViewMatrix", mat4{});
    program.setUniform("ProjectionMatrix", mat4{});
  }

  /*!
   * Window destructor that will free OpenGL resources
   */
  ~GradientWindow() override {
    // Clean up all OpenGL objects
    glDeleteBuffers(1, &cbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    // Set gray background
    glClearColor(.5, .5, .5, 0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) vertex_buffer.size());
  }
};

int main() {
  // Create a window with OpenGL 3.3 enabled
  GradientWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
