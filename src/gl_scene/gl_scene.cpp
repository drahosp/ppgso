// Example gl_scene
// - Demonstrates the concept of a scene
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <map>
#include <list>

#include <ppgso/ppgso.h>

#include "camera.h"
#include "scene.h"
#include "generator.h"
#include "player.h"
#include "space.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class SceneWindow : public Window {
private:
  Scene scene;

  // Set up the scene
  void InitializeScene() {
    scene.objects.clear();

    // Create a camera
    auto camera = make_shared<Camera>(60.0f, 1.0f, 0.1f, 100.0f);
    camera->position.z = -15.0f;
    scene.camera = camera;

    // Add space background
    auto space = make_shared<Space>();
    scene.objects.push_back(space);

    // Add generator to scene
    auto generator = make_shared<Generator>();
    generator->position.y = 10.0f;
    scene.objects.push_back(generator);

    // Add player to the scene
    auto player = make_shared<Player>();
    player->position.y = -6;
    scene.objects.push_back(player);
  }

public:
  SceneWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    HideCursor();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    InitializeScene();
  }

  // Keyboard press event handler
  void onKey(int key, int scanCode, int action, int mods) {
    scene.keyboard[key] = action;

    // Reset
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      InitializeScene();
    }
  }

  // Mouse move event handler
  void onCursorPos(double cursorX, double cursorY) {
    scene.cursor.x = cursorX;
    scene.cursor.y = cursorY;
  }

  void onPool() {
    // Track time
    static float time = (float)glfwGetTime();

    // Compute time delta
    float dt = (float)glfwGetTime() - time;
    time = (float)glfwGetTime();

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render all objects
    scene.Update(dt);
    scene.Render();
  }
};

int main() {
  // Initialize our window
  auto window = SceneWindow("gl_scene", SIZE, SIZE);

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
