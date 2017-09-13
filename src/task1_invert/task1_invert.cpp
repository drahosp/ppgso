// Task 1 - Load a 512x512 image
//        - Invert colors
//        - save as result.raw
#include <fstream>
#include <iostream>
#include <array>

using namespace std;

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
// Note: each channel is range <0, 255>
//       uint8_t or char would be signed so the range would be <-128, 127>
struct Pixel {
  uint8_t r, g, b;
};

int main()
{
  // Use 2D array for framebuffer, using std::array container
  using Framebuffer = array<array<Pixel,SIZE>, SIZE>;

  // Initialize a framebuffer on heap
  Framebuffer framebuffer{};

  // - Load a 512x512 image
  auto file = fopen("lena.raw", "rb");
  if (!file) {
    cout << "Cannot load lena.raw." << endl;
    return EXIT_FAILURE;
  }
  fread(framebuffer.data(), sizeof(Pixel) * SIZE * SIZE, 1, file); // Read directly to framebuffer
  fclose(file);

  // - Invert colors
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      framebuffer[x][y].r = (uint8_t)(255 - framebuffer[x][y].r);
      framebuffer[x][y].g = (uint8_t)(255 - framebuffer[x][y].g);
      framebuffer[x][y].b = (uint8_t)(255 - framebuffer[x][y].b);
    }
  }

  // - save as result.rgb, alternatively use fopen/fwrite etc.
  cout << "Generating task1_invert.raw file ..." << endl;
  ofstream raw("task1_invert.raw", ios::binary);
  raw.write((char*)framebuffer.data(), sizeof(Pixel) * SIZE * SIZE);
  raw.close();

  cout << "Done." << endl;

  return EXIT_SUCCESS;
}
