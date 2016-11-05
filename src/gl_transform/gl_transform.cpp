// Example gl_transform
// - This example demonstrates basic 2D transformations
// - Notice that the glm library uses column major order when declaring matrices
// - Try to combilne multiple transformation using multiplication

#include <iostream>

#include <ppgso/ppgso.h>
#include <glm/mat3x3.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "gl_transform_vert.h"
#include "gl_transform_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class TransformWindow : public Window {
private:
  Shader program{gl_transform_vert, gl_transform_frag};
  Texture texture{"lena.rgb", 512, 512};
  Mesh quad{program, "quad.obj"};

  mat3 GetTransformation(float time) {
    // Create transformation matrix
    // NOTE: glm matrices are declared column major !

    // identity
    auto transform = mat3({
                              1.0, 0.0, 0.0,
                              0.0, 1.0, 0.0,
                              0.0, 0.0, 1.0
                          });

    // scale
//   transform *= mat3({
//      sin(time), 0.0, 0.0,
//      0.0, sin(time), 0.0,
//      0.0, 0.0, 1.0
//    });

    // squash
//   transform *= mat3({
//     sin(time), 0.0, 0.0,
//     0.0, cos(time), 0.0,
//     0.0, 0.0, 1.0
//   });

    // rotate
//    transform *= mat3({
//      cos(time), sin(time), 0.0,
//      -sin(time), cos(time), 0.0,
//      0.0, 0.0, 1.0
//    });

    // translate
//   transform *= mat3({
//      1.0, 0.0, 0.0,
//     0.0, 1.0, 0.0,
//     sin(time)/2.0, cos(time)/2.0, 1.0
//   });

    // Rotate around top right corner
//   transform *= mat3({
//     // Move back
//     1.0, 0.0, 0.0,
//     0.0, 1.0, 0.0,
//     1.0, 1.0, 1.0
//   }) * mat3({
//     // Rotate
//     cos(time), sin(time), 0.0,
//     -sin(time), cos(time), 0.0,
//     0.0, 0.0, 1.0
//   }) * mat3({
//     // Move to origin
//     1.0, 0.0, 0.0,
//     0.0, 1.0, 0.0,
//     -1.0, -1.0, 1.0
//   });

    // Rotate around top right corner using glm functions
    auto rotation_point = vec2{1,1};
    transform = translate(mat3{}, rotation_point) * rotate(mat3{}, time) * glm::translate(mat3{}, -rotation_point);

    return transform;
  }

public:
  TransformWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Set program input for texture uniform
    program.SetTexture(texture, "Texture");
  }

  void onPool() {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = (float) glfwGetTime();

    // Generate and set the transformation matrix
    program.SetMatrix(GetTransformation(time), "ModelView");

    // Render geometry
    quad.Render();
  }
};


int main() {
  // Create our window
  auto window = TransformWindow{"gl_transform", SIZE, SIZE};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
