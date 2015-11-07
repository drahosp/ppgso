// Example raw_gradient
// - Illustrates the concept of a framebuffer
// - We do not really need any libraries or hardware to do computer graphics
// - In this case the framebuffer is simply saved as a raw RGB and TGA image

#include <iostream>
#include <fstream>

// A header implementation of a TGAImage object
#include "tga.h"

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
  Pixel framebuffer[SIZE][SIZE];

  // Example: Generate a simple gradient
  for (unsigned int x = 0; x < SIZE; ++x) {
    for (unsigned int y = 0; y < SIZE; ++y) {
      framebuffer[x][y].r = ((unsigned char)((unsigned int)x / 2));
      framebuffer[x][y].g = ((unsigned char)((unsigned int)y / 2));
      framebuffer[x][y].b = 0;
    }
  }

  // Task1: Load RAW image file here instead
  // Task2: Apply a convolution filter to the loaded image
  // Task3: Merge multiple images using alpha blending
  // Task4: Draw lines and Bezier curves

  // Save the raw image to a file
  std::cout << "Generating result.rgb file ..." << std::endl;
  std::ofstream raw("result.rgb", std::ios::binary);
  raw.write((char *)framebuffer, sizeof(framebuffer));
  raw.close();

  // Helper object to save the framebuffer as TGA image
  std::cout << "Generating recult.tga file ..." << std::endl;
  TGAImage tga(SIZE, SIZE, 3, framebuffer);
  tga.write_tga_file("result.tga", true);

  std::cout << "Done." << std::endl;
  return EXIT_SUCCESS;
}
