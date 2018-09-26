#include <algorithm>
#include "image.h"

using namespace std;
using namespace ppgso;

uint8_t clamp(float value) {
  return (uint8_t) (std::min(std::max(value, 0.0f), 1.0f) * 255.0f);
}

Image::Image(int width, int height) : width{width}, height{height} {
  framebuffer.resize((size_t) (width * height));
}

std::vector<Image::Pixel>& Image::getFramebuffer() {
  return framebuffer;
}

Image::Pixel& Image::getPixel(int x, int y) {
  return framebuffer[x+y*width];
}

void Image::setPixel(int x, int y, const Image::Pixel& color) {
  framebuffer[x+y*width] = color;
}

void Image::clear(const Image::Pixel &color) {
  framebuffer = vector<Pixel>(framebuffer.size(), color);
}

void Image::setPixel(int x, int y, int r, int g, int b) {
  setPixel(x,y,{(uint8_t)r, (uint8_t)g, (uint8_t)b});
}

void Image::setPixel(int x, int y, float r, float g, float b) {
  setPixel(x,y,{clamp(r), clamp(g), clamp(b)});
}
