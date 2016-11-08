// Task 2 - Implement Bresenham drawing alg.
//        - Draw a star using lines
//        - Save as result.raw
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>

using namespace std;

// Size of the framebuffer
const unsigned int SIZE = 512;
const float PI = 3.141598f;

// A simple RGB struct will represent a pixel in the framebuffer
// Note: unsigned char is range <0, 255>
//       signed char is range <-128, 127>
struct Pixel {
  unsigned char r, g, b;
};

struct Point {
  int x,y;
};

Pixel framebuffer[SIZE][SIZE];

// Bresenham drawing algorithm
void drawLine(Point& from, Point& to) {
  int x = from.x, y = from.y;
  int dx =  abs(to.x-from.x), sx = from.x<to.x ? 1 : -1;
  int dy = -abs(to.y-from.y), sy = from.y<to.y ? 1 : -1;
  int err = dx+dy, e2;

  while(true){
    framebuffer[x][y] = Pixel{255,255,255};
    if (x==to.x && y==to.y) break;
    e2 = 2*err;
    if (e2 >= dy) { err += dy; x += sx; }
    if (e2 <= dx) { err += dx; y += sy; }
  }
}

int main()
{
  // Initialize framebuffer to 0
  memset(framebuffer, 0, sizeof(framebuffer));

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
    drawLine(points[i], points[i+1]);

  // - Save as result.raw, alternatively use fopen/fwrite etc.
  cout << "Generating result.raw file ..." << endl;
  ofstream raw("result.raw", ios::binary);
  raw.write(reinterpret_cast<char*>(framebuffer), sizeof(Pixel) * SIZE * SIZE);
  raw.close();

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}

