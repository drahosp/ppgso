// Example gl5_projection
// - Demonstrates the use of perspective projection
// - Animates object rotation
// - Useful for demonstrating culling and depth test concepts

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ppgso/ppgso.h>

#include "gl5_projection_vert.h"
#include "gl5_projection_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class ProjectionWindow : public Window {
private:
  // Load shader program, geometry and texture
  Shader program = {gl5_projection_vert, gl5_projection_frag};
  Mesh quad = {program, "quad.obj"};
  Texture texture = {"lena.bmp"};

  // Model matrix for each quad;
  mat4 quad1Mat, quad2Mat;
public:
  ProjectionWindow() : Window{"gl5_projection", SIZE, SIZE} {
    // Set up OpenGL options
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
//  glEnable(GL_CULL_FACE);
//  glFrontFace(GL_CCW);
//  glCullFace(GL_BACK);

    // Set texture as program uniform input
    program.SetTexture(texture, "Texture");

    // Initialize projection
    // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
    // You can think of this as the camera objective settings
    auto projection = perspective( (PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);
    program.SetMatrix(projection, "ProjectionMatrix");

    // Create view matrix (translate camera a bit backwards, so we can see the geometry)
    // This can be seen as the camera position/rotation in space
    auto view = translate(mat4{}, {0.0f, 0.0f, -3.0f});
    program.SetMatrix(view, "ViewMatrix");

    // Quad positions
    // Coordinates in world coordinates
    quad1Mat = translate(mat4{}, {0,0,1});
    quad2Mat = translate(mat4{}, {0,0,-1});
  }

  void onIdle() override {
    // Update time and create a rotation matrix
    float time = (float)glfwGetTime();
    auto rotateMat = rotate(mat4{}, time, {0,1,0});

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw two quads
    program.SetMatrix(rotateMat * quad1Mat, "ModelMatrix");
    quad.Render();

    program.SetMatrix(rotateMat * quad2Mat, "ModelMatrix");
    quad.Render();
  }
};

int main() {
  // Initialize our window
  auto window = ProjectionWindow{};

  // Pool window events
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
