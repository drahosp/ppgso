// Task 2 - Implement a convolution filter
//        - Define a convolution filter
//        - Handle image boundary correctly as specified in assignment
//        - Correctly handle color manipulation

#include <ppgso/ppgso.h>

using namespace std;
using namespace ppgso;

const int kernel_height = 5;
const int kernel_width = 5;
float kernel[kernel_height][kernel_width] {
    // TODO: Define the kernel
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
  // We will use ppgso::Image for the source and destination framebuffer this time
  ppgso::Image source = image::loadBMP("lena.bmp");
  ppgso::Image destination(source.width, source.height);

  // Traverse the source image, use Image::getPixel to get source samples
  for(int y = 0; y < source.height; ++y) {
    for(int x = 0; x < source.width; ++x) {
      // Store sum of colors
      float r = 0, g = 0, b = 0;

      // TODO: Compute convolution

      // Store the result divided by factor
      destination.setPixel(x, y, r, g, b);
    }
  }

  // Save as result
  cout << "Generating task2_convolution.bmp file ..." << endl;
  image::saveBMP(destination, "task2_convolution.bmp");

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}
