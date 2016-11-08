// Task 1 - Load a 512x512 image
//        - Invert colors
//        - Save as result.raw
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
  auto file = fopen("lena.raw", "rb");
  if (!file) {
    cout << "Cannot load lena.raw." << endl;
    delete[] framebuffer;
    return EXIT_FAILURE;
  }
  fread(framebuffer, sizeof(Pixel) * SIZE * SIZE, 1, file); // Read directly to framebuffer
  fclose(file);

  // - Invert colors
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      framebuffer[x][y].r = (unsigned char)(255 - framebuffer[x][y].r);
      framebuffer[x][y].g = (unsigned char)(255 - framebuffer[x][y].g);
      framebuffer[x][y].b = (unsigned char)(255 - framebuffer[x][y].b);
    }
  }

  // - Save as result.rgb, alternatively use fopen/fwrite etc.
  cout << "Generating result.raw file ..." << endl;
  ofstream raw("result.raw", ios::binary);
  raw.write(reinterpret_cast<char*>(framebuffer), sizeof(Pixel) * SIZE * SIZE);
  raw.close();

  cout << "Done." << endl;
  delete[] framebuffer;
  return EXIT_SUCCESS;
}
