// Example raw_gradient
// - Illustrates the concept of a framebuffer
// - We do not really need any libraries or hardware to do computer graphics
// - In this case the framebuffer is simply saved as a raw RGB and TGA image

#include <iostream>
#include <fstream>

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
// Note: unsigned char is range <0, 255>
//       signed char is range <-128, 127>
struct Pixel {
  unsigned char r, g, b;
};

int main() {
  // Initialize a framebuffer
  // NOTE: The framebuffer is allocated on the stack. For bigger sizes this should be changed to a heap allocation.
  Pixel framebuffer[SIZE][SIZE];

  // Example: Generate a simple gradient
  for (unsigned int x = 0; x < SIZE; ++x) {
    for (unsigned int y = 0; y < SIZE; ++y) {
      framebuffer[x][y].r = static_cast<unsigned char>(x / 2);
      framebuffer[x][y].g = static_cast<unsigned char>(y / 2);
      framebuffer[x][y].b = 0;
    }
  }

  // Task1: Load RAW image file here instead

  // Save the raw image to a file
  std::cout << "Generating result.rgb file ..." << std::endl;
  std::ofstream raw("result.rgb", std::ios::binary);
  raw.write(reinterpret_cast<char *>(framebuffer), sizeof(framebuffer));
  raw.close();

  std::cout << "Done." << std::endl;
  return EXIT_SUCCESS;
}
