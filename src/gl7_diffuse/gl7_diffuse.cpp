// Example gl_diffuse
// - Demonstration of simple diffuse lighting with one directional light source

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <ppgso/ppgso.h>

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

/*!
 * Custom window for displaying a mesh with diffuse lighting
 */
class DiffuseWindow : public Window {
private:
  Shader program = {diffuse_vert_glsl, diffuse_frag_glsl};
  Texture texture = {image::loadBMP("missile.bmp")};
  Mesh object = {"missile.obj"};

public:
  /*!
   * Create new custom window
   */
  DiffuseWindow() : Window{"gl7_diffuse", SIZE, SIZE} {
    // Set camera position/rotation - for example, translate camera a bit backwards (positive value in Z axis), so we can see the objects
    auto cameraMat = translate(mat4{}, {0.0f, 0.0f, -1.0f});
    program.setUniform("ViewMatrix", cameraMat);

    // Set camera position with perspective projection
    program.setUniform("ProjectionMatrix", perspective((PI / 180.f) * 60.0f, 1.0f, 0.1f, 10.0f));

    // Set the light direction, assumes simple white directional light
    program.setUniform("LightDirection", normalize(vec3{1.0f, -1.0f, 1.0f}));

    // Set texture as program input
    program.setUniform("Texture", texture);

    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
  }

  /*!
   * Handles pressed key when the window is focused
   * @param key Key code of the key being pressed/released
   * @param scanCode Scan code of the key being pressed/released
   * @param action Action indicating the key state change
   * @param mods Additional modifiers to consider
   */
  void onKey(int key, int scanCode, int action, int mods) override {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      // Set new random light direction
      program.setUniform("LightDirection", (vec3)sphericalRand(1.0));

    }
  }

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get time
    auto time = glfwGetTime();

    // Create object matrix that rotates in time
    auto sphereMat = rotate(mat4{}, (float)time, {0.5f, 1.0f, 0.0f});

    // Set the matrix as model matrix for current program
    program.setUniform("ModelMatrix", sphereMat);

    // Render object
    object.render();
  }
};

int main() {
  // Create a window with OpenGL 3.3 enabled
  DiffuseWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
