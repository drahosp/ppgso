// Example gl_mesh
// - Displays geometry that is loaded from OBJ files encapsulated in a mesh object
// - Demonstrates basic use of keyboard events (press A to start/stop animation) using inheritance from ppgso::Window
// - Implements object transformation based on mouse movement
// - Combines parallel and orthographic camera projection

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include "gl_mesh_frag.h"
#include "gl_mesh_vert.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

// Our custom application
class MeshWindow : public Window {
private:
  Shader program{gl_mesh_vert, gl_mesh_frag};

  Texture sphereTexture{"sphere.rgb", 256, 256};
  Texture cursorTexture{"lena.rgb", 512, 512};

  Mesh sphere{program, "sphere.obj"};
  Mesh cursor{program, "quad.obj"};

  bool animationEnabled = true;
  double cursorX = 0.0f;
  double cursorY = 0.0f;

public:
  // Initialize our window
  MeshWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    HideCursor();

    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
  }

  // Handle key 'A' to pause the animation
  void onKey(int key, int scanCode, int action, int mods) {
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      animationEnabled = !animationEnabled;
    }
  }

  // Update cursor position (screen coordinates <0, SIZE> to OpenGL viewport coordinates <-1,1>)
  void onCursorPos(double cursorX, double cursorY) {
    this->cursorX = (cursorX / ((double) SIZE) * 2) - 1;
    this->cursorY = -((cursorY / ((double) SIZE) * 2) - 1);
  }

  // Display the scene
  void onPool() {
    float time = 0;

    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Animate using time when activated
    if (animationEnabled)
      time = (float) glfwGetTime();

    // Create object matrices
    auto centerSphereMat = rotate(mat4{}, time, {0.5f, 1.0f, 0.0f});
    auto smallSphereMat = translate(mat4{}, {sin(time), cos(time), 0});
    smallSphereMat = scale(smallSphereMat, {0.5f, 0.5f, 0.5f});

    // Camera position/rotation - for example, translate camera a bit backwards (positive value in Z axis), so we can see the objects
    auto cameraMat = translate(mat4{}, {0.0f, 0.0f, -2.5f});
    program.SetMatrix(cameraMat, "ViewMatrix");

    // Update camera position with perspective projection
    program.SetMatrix(perspective((PI / 180.f) * 60.0f, 1.0f, 0.1f, 10.0f), "ProjectionMatrix");

    // Render objects
    program.SetTexture(sphereTexture, "Texture");

    // Central sphere
    program.SetMatrix(centerSphereMat, "ModelMatrix");
    sphere.Render();

    // Smaller sphere
    program.SetMatrix(smallSphereMat, "ModelMatrix");
    sphere.Render();

    // Draw cursor using orthographic/parallel projection
    program.SetMatrix(ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1000.0f, 1000.0f), "ProjectionMatrix");

    // Create object matrix
    auto cursorMat = translate(mat4{}, {cursorX, cursorY, 0.0f});
    cursorMat = scale(cursorMat, {0.1f, 0.1f, 0.1f});

    // Render objects
    program.SetTexture(cursorTexture, "Texture");
    program.SetMatrix(cursorMat, "ModelMatrix");
    cursor.Render();
  }
};

int main() {
  // Create instance of our mesh window
  auto window = MeshWindow{"gl_mesh", SIZE, SIZE};

  // Pool events in loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
