// Example gl6_mesh
// - Displays geometry that is loaded from Wavefront OBJ files encapsulated in a mesh object
// - Implements object transformation based on mouse movement
// - Combines parallel and orthographic camera projection
// - SPACE stops/resumes the animation

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

/*!
 * Custom window for displaying multiple meshes projections
 */
class MeshWindow : public Window {
private:
  // Initialize resources
  Shader program = {texture_vert_glsl, texture_frag_glsl};

  Texture sphereTexture = {image::loadBMP("sphere.bmp")};
  Texture cursorTexture = {image::loadBMP("lena.bmp")};

  Mesh sphere = {"sphere.obj"};
  Mesh cursor = {"quad.obj"};
  Mesh cube = {"cube.obj"};

  bool animationEnabled = true;
  double cursorX = 0.0;
  double cursorY = 0.0;

  float time = 0.0f;
public:
  /*!
   * Construct our new custom window and set up OpenGL
   */
  MeshWindow() : Window{"gl6_mesh", SIZE, SIZE} {
    hideCursor();

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
      animationEnabled = !animationEnabled;
    }
  }

  /*!
   * Handle cursor position changes
   * @param cursorX Mouse horizontal position in window coordinates
   * @param cursorY Mouse vertical position in window coordinates
   */
  void onCursorPos(double cursorX, double cursorY) override {
    // Convert position from screen coordinates <0, SIZE> to OpenGL viewport coordinates <-1,1>
    this->cursorX = (cursorX / ((double) SIZE) * 2) - 1;
    this->cursorY = -((cursorY / ((double) SIZE) * 2) - 1);
  }

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    // Animate using time when activated
    if (animationEnabled) time = (float) glfwGetTime();

    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create object matrices
    auto cubeMat = rotate(mat4{1.0f}, time, {0.5f, 1.0f, 0.0f});
    auto sphereMat = translate(mat4{1.0f}, {sin(time), cos(time), 0});
    sphereMat = scale(sphereMat, {0.5f, 0.5f, 0.5f});

    // Camera position/rotation - for example, translate camera a bit backwards (positive value in Z axis), so we can see the objects
    auto cameraMat = translate(mat4{1.0f}, {0.0f, 0.0f, -2.5f});
    program.setUniform("ViewMatrix", cameraMat);

    // Update camera position with perspective projection
    program.setUniform("ProjectionMatrix", perspective((PI / 180.f) * 60.0f, 1.0f, 0.1f, 10.0f));

    // Render objects
    program.setUniform("Texture", sphereTexture);

    // Central box
    program.setUniform("ModelMatrix", cubeMat);
    cube.render();

    // Smaller sphere
    program.setUniform("ModelMatrix", sphereMat);
    sphere.render();

    // Draw cursor using orthographic/parallel projection
    program.setUniform("ProjectionMatrix", ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1000.0f, -1000.0f));

    // Create object matrix
    auto cursorMat =  translate(mat4{1.0f}, {cursorX, cursorY, 0.0f}) * scale(mat4{1.0f}, {0.1f, 0.1f, 0.1f});

    // Render objects
    program.setUniform("Texture", cursorTexture);
    program.setUniform("ModelMatrix", cursorMat);
    cursor.render();
  }
};

int main() {
  // Create instance of our mesh window
  MeshWindow window;

  // Poll events in loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
