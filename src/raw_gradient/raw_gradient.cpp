// Example raw_gradient
// - Illustrates the concept of a framebuffer
// - We do not really need any libraries or hardware to do computer graphics
// - In this case the framebuffer is simply saved as a raw RGB and TGA image

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <cstring>

using namespace std;

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
// Note: unsigned char is range <0, 255>
//       signed char is range <-128, 127>
struct Pixel {
  unsigned char r,g,b;
};

int main() {
  // Initialize a framebuffer
  // NOTE: The framebuffer is allocated on the heap and needs to be freed manually.
  // Allocating on the stack may cause stack overflow depending on the size of the image.
  auto framebuffer = new Pixel[SIZE][SIZE];

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
  cout << "Generating result.rgb file ..." << endl;
  ofstream raw("result.rgb", ios::binary);
  raw.write(reinterpret_cast<char *>(framebuffer), sizeof(Pixel)*SIZE*SIZE);
  raw.close();

  delete[](framebuffer);

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}
