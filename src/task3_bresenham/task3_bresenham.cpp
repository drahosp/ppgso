// Task 3 - Implement Bresenham drawing alg.
//        - Draw a star using lines
//        - Make use of std::vector to define the shape
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include <ppgso/ppgso.h>

using namespace std;
using namespace ppgso;

// Size of the framebuffer
const unsigned int SIZE = 512;

struct Point {
  int x,y;
};

// Bresenham drawing algorithm
void drawLine(ppgso::Image& framebuffer, Point& from, Point& to) {
  // TODO: Implement Bresenham drawing algorithm
}

int main()
{
  // Use ppgso::Image as our framebuffer
  ppgso::Image framebuffer(SIZE, SIZE);

  // TODO: Generate star points
  vector<Point> points;

  // Draw lines
  for(unsigned int i = 0; i < points.size() - 1; i++)
    drawLine(framebuffer, points[i], points[i+1]);

  // Save the result
  cout << "Generating task3_bresenham.bmp file ..." << endl;
  image::saveBMP(framebuffer, "task3_bresenham.bmp");

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}

