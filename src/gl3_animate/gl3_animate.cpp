// Example gl3_animate
// - Demonstrates the use of a dynamically generated texture content on the CPU
// - Displays the generated content as texture on a quad using OpenGL
// - Basic animation achieved by incrementing a parameter used in the image generation

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

/*!
 * Custom window that will update its contents to create animation
 */
class AnimateWindow : public Window {
private:
  // Create shading program from included shader sources
  Shader program = {texture_vert_glsl, texture_frag_glsl};

  // Load a quad mesh
  Mesh quad = {"quad.obj"};

  // Initialize texture
  Texture texture = {SIZE, SIZE};

  /*!
   * Update OpenGL texture with new animation frame
   * @param texture Texture to update
   * @param time Time to generate animation frame for
   */
  void updateTexture(Texture &texture, double time) {
    // Draw something to the texture
    double cx = sin(time);
    double cy = cos(time * 0.9);

    #pragma omp parallel for
    for (int y = 0; y < texture.image.height; y++) {
      for (int x = 0; x < texture.image.width; x++) {
        auto& pixel = texture.image.getPixel(x, y);
        double fx = (float) x / (float) (texture.image.width) - .5;
        double fy = (float) y / (float) (texture.image.height) - .5;
        double dist = sqrt(pow(fx - cx, 2.0) + pow(fy - cy, 2.0));

        pixel.r = (uint8_t) (sin(dist * 45.0) * 127 + 128);
        pixel.g = (uint8_t) (sin(dist * 44.0) * 127 + 128);
        pixel.b = (uint8_t) (sin(dist * 46.0) * 127 + 128);
      }
    }
    // Update the OpenGL texture content
    texture.update();
  }

public:
  /*!
   * Construct a new Window and initialize shader uniform variables
   */
  AnimateWindow() : Window{"gl3_animate", SIZE, SIZE} {
    // Pass the texture to the program as uniform input called "Texture"
    program.setUniform("Texture", texture);

    // Set Matrices to identity so there are no projections/transformations applied in the vertex shader
    program.setUniform("ModelMatrix", mat4{1.0f});
    program.setUniform("ViewMatrix", mat4{1.0f});
    program.setUniform("ProjectionMatrix", mat4{1.0f});
  }

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    // Generate texture content
    auto time = glfwGetTime();
    updateTexture(texture, time);

    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the quad geometry
    quad.render();
  }
};

int main() {
  // Create a window with OpenGL 3.3 enabled
  AnimateWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
