// Task 1 - Load a 512x512 image
//        - Invert colors
//        - Save as result.rgb
#include <fstream>
#include <iostream>
#include <array>
#include <cstdio>

using namespace std;

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
// Note: unsigned char is range <0, 255>
//       signed char is range <-128, 127>
struct Pixel {
  unsigned char r, g, b;
};

int main()
{
  // Initialize a framebuffer
  auto framebuffer = new Pixel[SIZE][SIZE];

  // - Load a 512x512 image
  auto file = fopen("lena.rgb", "rb");
  if (!file) {
    cout << "Cannot load lena.rgb." << endl;
    delete[] framebuffer;
    return EXIT_FAILURE;
  }
  fread(framebuffer, sizeof(Pixel) * SIZE * SIZE, 1, file); // Read directly to framebuffer
  fclose(file);

  // - Invert colors
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      framebuffer[x][y].r = 255 - framebuffer[x][y].r;
      framebuffer[x][y].g = 255 - framebuffer[x][y].g;
      framebuffer[x][y].b = 255 - framebuffer[x][y].b;
    }
  }

  // - Save as result.rgb, alternatively use fopen/fwrite etc.
  cout << "Generating result.rgb file ..." << endl;
  ofstream raw("result.rgb", ios::binary);
  raw.write(reinterpret_cast<char*>(framebuffer), sizeof(Pixel) * SIZE * SIZE);
  raw.close();

  cout << "Done." << endl;
  delete[] framebuffer;
  return EXIT_SUCCESS;
}
