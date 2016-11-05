// Example gl_animate
// - Demonstrates the use of a dynamically generated texture content on the CPU
// - Displays the generated content as texture on a quad using OpenGL
// - Basic animation achieved by incrementing a parameter used in the image generation

#include <iostream>
#include <cmath>

#include <ppgso/ppgso.h>

#include "gl_animate_vert.h"
#include "gl_animate_frag.h"

using namespace std;
using namespace ppgso;

const unsigned int SIZE = 512;

class AnimateWindow : public Window {
private:
  // Create shading program from included shader sources
  Shader program{gl_animate_vert, gl_animate_frag};

  // Load a quad mesh
  Mesh quad{program, "quad.obj"};

  // Initialize texture
  Texture texture{SIZE, SIZE};

  // Update texture content
  void UpdateTexture(Texture &texture, float time) {
    // Draw something to the texture
    float cx = sin(time);
    float cy = cos(time*0.9f);

    for(unsigned int x = 0; x < texture.width; x++) {
      for(unsigned int y = 0; y < texture.height; y++) {
        auto pixel = texture.GetPixel(x, y);
        float fx = (float)x / (float)(texture.width) - .5f;
        float fy = (float)y / (float)(texture.height) - .5f;
        float dist = sqrt(pow(fx - cx, 2.0f) + pow(fy - cy, 2.0f));

        pixel->r = (unsigned char) (sin(dist * 45.0f) * 127 + 128);
        pixel->g = (unsigned char) (sin(dist * 44.0f) * 127 + 128);
        pixel->b = (unsigned char) (sin(dist * 46.0f) * 127 + 128);
      }
    }
    texture.Update();
  }

public:
  AnimateWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Pass the texture to the program as uniform input called "Texture"
    program.SetTexture(texture, "Texture");
  }

  void onPool() {
    // Generate texture content
    float time = (float)glfwGetTime();
    UpdateTexture(texture, time);

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the quad geometry
    quad.Render();
  }
};

int main() {
  // Create a window with OpenGL 3.3 enabled
  auto window = AnimateWindow{"gl_animate", SIZE, SIZE};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
