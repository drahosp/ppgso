// Example gl_diffuse
// - Demonstration of simple diffuse lighting with one directional light source

#include <glm/glm.hpp>

#include <ppgso/ppgso.h>

#include "gl_diffuse_frag.h"
#include "gl_diffuse_vert.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;
const float PI = pi<float>();

class DiffuseWindow : public Window {
private:
  Shader program{gl_diffuse_vert, gl_diffuse_frag};
  Texture texture{"asteroid.rgb", 512, 512};
  Mesh object{program, "asteroid.obj"};

public:
  DiffuseWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Set camera position/rotation - for example, translate camera a bit backwards (positive value in Z axis), so we can see the objects
    auto cameraMat = translate(mat4{}, {0.0f, 0.0f, -2.5f});
    program.SetMatrix(cameraMat, "ViewMatrix");

    // Set camera position with perspective projection
    program.SetMatrix(perspective((PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f), "ProjectionMatrix");

    // Set the light direction
    program.SetVector(vec3{1.0f,-1.0f,1.0f}, "LightDirection");

    // Set texture as program input
    program.SetTexture(texture, "Texture");

    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
  }

  void onPool() {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get time
    float time = (float) glfwGetTime();

    // Create object matrix that rotates in time
    auto sphereMat = rotate(mat4{}, time, {0.5f, 1.0f, 0.0f});

    // Set the matrix as model matrix for current program
    program.SetMatrix(sphereMat, "ModelMatrix");

    // Render object
    object.Render();
  }
};

int main() {
  // Create a window with OpenGL 3.3 enabled
  auto window = DiffuseWindow{"gl_diffuse", SIZE, SIZE};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
