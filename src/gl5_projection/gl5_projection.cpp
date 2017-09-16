// Example gl5_projection
// - Demonstrates the use of perspective and parallel projection
// - Animates object rotation
// - Useful for demonstrating culling and depth test concepts

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

/*!
 * Custom window for demonstrating projections
 */
class ProjectionWindow : public Window {
private:
  // Load shader program, geometry and texture
  Shader program = {texture_vert_glsl, texture_frag_glsl};
  Mesh quad = {"quad.obj"};
  Texture texture = {image::loadBMP("lena.bmp")};

  // Model matrix for each quad
  mat4 quad1ModelMatrix, quad2ModelMatrix;

  // Mode switch
  enum class Mode {
    PERSPECTIVE,
    PARALLEL,
    END,
  } mode = Mode::PERSPECTIVE;

  /*!
   * Set ProjectionMatrix and ViewMatrix depending on the current mode
   */
  void setProjection() {
    switch (mode) {
      case Mode::PERSPECTIVE:
        // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
        // You can think of this as the camera objective settings
        program.setUniform("ProjectionMatrix", perspective((PI / 180.f) * 60.0f, 1.0f, 0.1f, 10.0f));

        // Create view matrix (translate camera a bit backwards, so we can see the geometry)
        program.setUniform("ViewMatrix", translate(mat4{}, {0.0f, 0.0f, -3.0f}));
        break;
      case Mode::PARALLEL:
        // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
        // You can think of this as the camera objective settings
        program.setUniform("ProjectionMatrix", ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f));

        // Create view matrix using the glm::lookAt function
        // This can be seen as the camera position/rotation in space
        program.setUniform("ViewMatrix", lookAt(vec3{2.0f, 2.0f, 2.0f}, vec3{0.0f, 0.0f, 0.0f}, vec3{0.0f, 1.0f, 0.0f}));
        break;
      default:
        break;
    }
  }

public:
  /*!
   * Create and set up new custom window
   */
  ProjectionWindow() : Window{"gl5_projection", SIZE, SIZE} {
    // Set up OpenGL options
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    //glCullFace(GL_BACK);

    // Set texture as program uniform input
    program.setUniform("Texture", texture);

    // Quad positions
    // Coordinates in world coordinates
    quad1ModelMatrix = translate(mat4{}, {0, 0, 1});
    quad2ModelMatrix = translate(mat4{}, {0, 0, -1});
  }

  /*!
   * Handles pressed key when the window is focused
   * @param key Key code of the key being pressed/released
   * @param scanCode Scan code of the key being pressed/released
   * @param action Action indicating the key state change
   * @param mods Additional modifiers to consider
   */
  void onKey(int key, int scanCode, int action, int mods) override {
    // When SPACE is pressed advance mode to the next state
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      int next_mode = ((int)mode+1) % (int)Mode::END;
      mode = Mode(next_mode % 8);
    }
  };

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    // Update time and create a rotation matrix
    auto time = glfwGetTime();
    auto rotateMat = rotate(mat4{}, (float)time, {0, 1, 0});

    // Set up projection and view matrix
    setProjection();

    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw two quads with rotation
    program.setUniform("ModelMatrix", rotateMat * quad1ModelMatrix);
    quad.render();

    program.setUniform("ModelMatrix", rotateMat * quad2ModelMatrix);
    quad.render();
  }
};

int main() {
  // Initialize our window
  ProjectionWindow window;

  // Poll window events
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
