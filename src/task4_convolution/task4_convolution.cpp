// Task 4 - Implement a convolution filter as GLSL vertex shader
//        - Define a 5x5 filter

#include <iostream>
#include <vector>
#include <fstream>

#include <ppgso/ppgso.h>

#include "task4_convolution_vert.h"
#include "task4_convolution_frag.h"

using namespace std;
using namespace ppgso;

const unsigned int SIZE = 512;

class ConvolutionWindow : public Window {
private:
  Shader program{task4_convolution_vert, task4_convolution_frag};
  Mesh quad{program, "quad.obj"};
  Texture texture{"lena.rgb", SIZE, SIZE};

public:
  ConvolutionWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Set program input for texture uniform
    program.SetTexture(texture, "Texture");
  }

  void onPool() {
    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render geometry
    quad.Render();
  }
};

int main() {
  // Create our window
  auto window = ConvolutionWindow{"task4_convolution", SIZE, SIZE};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
