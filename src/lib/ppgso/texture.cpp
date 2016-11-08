#include <iostream>
#include <fstream>

#include "texture.h"

using namespace std;
using namespace ppgso;

Texture::Texture(unsigned int width, unsigned int height) : width(width), height(height) {
  framebuffer.resize(width * height);
  initGL();
  Update();
}

Texture::Texture(const string &raw, unsigned int width, unsigned int height) : width(width), height(height) {
  load(raw, width, height);
  initGL();
  Update();
}

Texture::Texture(const string &bmp) {
  load(bmp);
  initGL();
  Update();
}

Texture::~Texture() {
  glDeleteTextures(1, &texture);
}

void Texture::initGL() {
  // Create new texture object
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set mipmaps
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture::Update() {
  Use();
  // Upload texture to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, framebuffer.data());

  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Use() {
  glBindTexture(GL_TEXTURE_2D, texture);
}

GLuint Texture::GetTexture() {
  return texture;
}

Texture::Pixel *Texture::Data() {
  return framebuffer.data();
}

Texture::Pixel *Texture::GetPixel(int x, int y) {
  return &framebuffer[x+y*width];
}

void Texture::load(const string &raw, unsigned int width, unsigned int height) {
  framebuffer.resize(width * height);

  // Open file stream
  ifstream image_stream(raw, ios::binary);

  if (!image_stream.is_open())
    cerr << "Could not open texture " << raw << endl;

  // Load the
  image_stream.read((char *)framebuffer.data(), framebuffer.size()*sizeof(Pixel));
  image_stream.close();
}

void Texture::load(const string &bmp) {
  BITMAPFILEHEADER bmpFileHeader;
  BITMAPINFOHEADER bmpInfoHeader;

  ifstream input_file(bmp, ios::binary);

  // Check headers
  if (!input_file.is_open())
    cerr << "Could not open BMP file. " << bmp << endl;

  input_file.read((char*)&bmpFileHeader, sizeof(BITMAPFILEHEADER));
  input_file.read((char*)&bmpInfoHeader, sizeof(BITMAPINFOHEADER));

  if(bmpFileHeader.bfType != 19778)
    cerr << "BMP file does not contain supported BMP format. " << bmp << endl;

  if(bmpInfoHeader.biBitCount != 24)
    cerr << "BMP file does not contain supported bit count. " << bmp << endl;

  if(bmpInfoHeader.biCompression != 0) // BI_RGB only
    cerr << "BMP file does not use expected compression method. " << bmp << endl;

  width = (unsigned int) bmpInfoHeader.biWidth;
  height = (unsigned int) abs(bmpInfoHeader.biHeight);
  bool flipped = bmpInfoHeader.biHeight < 0;

  if(width == 0 || height == 0)
    cerr << "BMP file does not contain any data. " << bmp << endl;
  
  // Load data
  input_file.seekg(bmpFileHeader.bfOffBits, input_file.beg);
  framebuffer.resize(width * height * sizeof(Pixel));

  // BMP uses padding for rows
  unsigned int row_padded = (width*3 + 3) & (~3);

  for(unsigned int j = 0; j < height; j++) {
    auto row_data = vector<Pixel>(row_padded);
    input_file.read((char*)row_data.data(), row_padded);
    for(unsigned int i = 0; i < width; i++) {
      auto pixel = row_data[i];
      swap(pixel.r, pixel.b);
      if (flipped) {
        framebuffer[i + j * width] = pixel;
      } else {
        framebuffer[i + (height - 1 - j) * width] = pixel;
      }
    }
  }
  input_file.close();
}

void Texture::Save(const string &bmp) {
  unsigned int row_padded = (width*3 + 3) & (~3);

  BITMAPFILEHEADER bmpFileHeader;
  bmpFileHeader.bfType = 19778;
  bmpFileHeader.bfSize = (unsigned int) (row_padded * height * sizeof(Pixel) + 122);
  bmpFileHeader.bfReserved1 = 0;
  bmpFileHeader.bfReserved2 = 0;
  bmpFileHeader.bfOffBits = 122;

  BITMAPINFOHEADER bmpInfoHeader;
  bmpInfoHeader.biSize = 108;
  bmpInfoHeader.biWidth = width;
  bmpInfoHeader.biHeight = height;
  bmpInfoHeader.biPlanes = 1;
  bmpInfoHeader.biBitCount = 24;
  bmpInfoHeader.biCompression = 0;
  bmpInfoHeader.biSizeImage = (unsigned int)(row_padded * height * sizeof(Pixel));
  bmpInfoHeader.biXPelsPerMeter = 2835;
  bmpInfoHeader.biYPelsPerMeter = 2835;
  bmpInfoHeader.biClrUsed = 0;
  bmpInfoHeader.biClrImportant = 0;

  ofstream output_file(bmp, ios::binary);

  if (!output_file.is_open())
    cerr << "Could not open BMP file for writing. " << bmp << endl;

  output_file.write((char *) &bmpFileHeader, sizeof(BITMAPFILEHEADER));
  output_file.write((char *) &bmpInfoHeader, sizeof(BITMAPINFOHEADER));

  // Prepare BRG output data by swapping RGB to BRG and mirroring along height
  output_file.seekp(bmpFileHeader.bfOffBits, output_file.beg);

  for (unsigned int j = 0; j < height; j++ ) {
    auto output_row = vector<Pixel>(row_padded);
    for (unsigned int i = 0; i < width; i++) {
      auto pixel = framebuffer[i + (height - 1 - j) * width];
      swap(pixel.r, pixel.b);
      output_row[i] = pixel;
    }
    output_file.write((char*)output_row.data(), row_padded);
  }

  output_file.close();
}
