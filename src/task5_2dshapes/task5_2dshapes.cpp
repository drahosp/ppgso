// Task 5 - Draw a 2D shape using polygons and animate it
//        - Encapsulate the shape using a class
//        - Animate the object position, rotation and scale.

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <ppgso/ppgso.h>

#include "task5_2dshapes_vert.h"
#include "task5_2dshapes_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

// Object to represent 2D shape
class Shape {
private:
  // 2D vectors define points/vertices of the shape
  vector<vec2> vetrices = {
      {-.3, 1},
      {.3,  1},
      {1, 0},
      {.3, -1},
      {-.3, -1},
      {-1, 0},
  };

  // Indices define triangles that index into vertices
  vector<GLuint> indices = {
      0, 1, 2,
      0, 2, 3,
      0, 3, 4,
      0, 4, 5,
  };

  // Program to associate with the object
  Shader program = {task5_2dshapes_vert, task5_2dshapes_frag};

  // These will hold the data and object buffers
  GLuint vao, vbo, ibo;
  mat3 model_matrix;
public:
  // Public attributes that define position, color ..
  vec2 position{0,0};
  float rotation{0};
  vec2 scale{1,1};
  vec3 color{1,0,0};

  // Initialize object data buffers
  Shape() {
    // Copy data to OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copy positions to gpu
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vetrices.size() * sizeof(vec2), vetrices.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto position_attrib = program.GetAttribLocation("pos");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy indices to gpu
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  };
  // Clean up
  ~Shape() {
    // Delete data from OpenGL
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  // Set the object transformation matrix
  void Update() {
    // Compute transformation by scaling, rotating and then translating the shape
    model_matrix = glm::translate(mat3{}, position)
                 * glm::rotate(mat3{}, rotation)
                 * glm::scale(mat3{}, scale);
  }

  // Draw polygons
  void Render(){
    // Update transformation and color uniforms in the shader
    program.Use();
    program.SetMatrix(model_matrix, "mat");
    program.SetVector(color, "color");

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
  };
};

class ShapeWindow : public Window {
private:
  Shape shape1, shape2;
public:
  ShapeWindow() : Window{"task5_2dshapes", SIZE, SIZE} {
    shape1.color = {1,0,0};
    shape2.color = {0,1,0};
  }

  void onIdle() {
    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Move and Render shape\    // Get time for animation
    float t = (float) glfwGetTime();

    // Set position to follow direction
    shape1.position = {sin(t), cos(t)};
    shape2.position = -shape1.position;

    // Set rotation and scale
    shape1.rotation = t*5.0f;
    shape2.rotation = -shape1.rotation;

    shape1.scale = {sin(t),sin(t)};
    shape2.scale = -shape1.scale;

    shape1.Update();
    shape2.Update();

    shape1.Render();
    shape2.Render();
  }
};

int main() {
  // Create our window
  auto window = ShapeWindow{};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
