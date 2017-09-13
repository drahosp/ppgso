// Task 3 - Implement Bresenham drawing alg.
//        - Draw a star using lines
//        - save as result.raw
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
  int x = from.x, y = from.y;
  int dx =  abs(to.x-from.x), sx = from.x<to.x ? 1 : -1;
  int dy = -abs(to.y-from.y), sy = from.y<to.y ? 1 : -1;
  int err = dx+dy, e2;

  while(true){
    framebuffer.setPixel(x,y,255,255,255);
    if (x==to.x && y==to.y) break;
    e2 = 2*err;
    if (e2 >= dy) { err += dy; x += sx; }
    if (e2 <= dx) { err += dx; y += sy; }
  }
}

int main()
{
  ppgso::Image framebuffer(SIZE, SIZE);

  // Generate star points
  vector<Point> points;
  float angle = 0;
  for(int i = 0; i <= 5; i++) {
    angle += PI + PI/5;
    int x = SIZE/2 + static_cast<int>(sin(angle)*SIZE/3.0f);
    int y = SIZE/2 - static_cast<int>(cos(angle)*SIZE/3.0f);
    points.push_back(Point{x, y});
  }

  // Draw lines
  for(unsigned int i = 0; i < points.size() - 1; i++)
    drawLine(framebuffer, points[i], points[i+1]);

  // - save as result.raw, alternatively use fopen/fwrite etc.
  cout << "Generating task3_bresenham.bmp file ..." << endl;
  image::saveBMP(framebuffer, "task3_bresenham.bmp");

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}

