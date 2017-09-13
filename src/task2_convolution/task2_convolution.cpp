// Task 2 - Implement a convolution filter
//        - Define a 5x5 filter

#include <ppgso/ppgso.h>

using namespace std;
using namespace ppgso;

const int kernel_height = 5;
const int kernel_width = 5;
float kernel[kernel_height][kernel_width] {
    { 0.0f,  1.0f,  1.0f,  1.0f, 1.0f},
    {-1.0f,  0.0f,  1.0f,  1.0f, 1.0f},
    {-1.0f, -1.0f,  0.0f,  1.0f, 1.0f},
    {-1.0f, -1.0f, -1.0f,  0.0f, 1.0f},
    {-1.0f, -1.0f, -1.0f, -1.0f, 0.0f}
};
float kernel_factor = 1.0f;
float kernel_bias = 0.0f;

// Clamp value to the <0, 1> range
inline float clamp(float value) {
  if(value > 1.0f) return 1.0f;
  if(value < 0) return 0;
  return value;
}

int main()
{
  // We will use ppgso::Image for the framebuffer this time
  ppgso::Image source = image::loadBMP("lena.bmp");
  ppgso::Image destination(source.width, source.height);

  // Traverse the source image, use Image::getPixel to get source samples
  for(int y = 0; y < source.height; ++y) {
    for(int x = 0; x < source.width; ++x) {
      // Store sum of colors
      float r = 0, g = 0, b = 0;

      // Gather convolution samples
      for(int v = 0; v < kernel_height; ++v) {
        for(int u = 0; u < kernel_width; ++u) {
          int sx = x + u - kernel_width / 2;
          int sy = y + v - kernel_height / 2;

          // Handle source edge
          if(sx < 0 || sx >= source.width || sy < 0 || sy >= source.height) continue;

          // Sum normalized colors from the sample
          auto& sample = source.getPixel(sx, sy);
          r += kernel[v][u] * ( sample.r / 255.0f );
          g += kernel[v][u] * ( sample.g / 255.0f );
          b += kernel[v][u] * ( sample.b / 255.0f );
        }
      }
      // Store the result divided by factor
      destination.setPixel(x, y,
                           clamp(r / kernel_factor + kernel_bias),
                           clamp(g / kernel_factor + kernel_bias),
                           clamp(b / kernel_factor + kernel_bias)
      );
    }
  }

  // Save as result
  cout << "Generating task2_convolution.bmp file ..." << endl;
  image::saveBMP(destination, "task2_convolution.bmp");

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}
