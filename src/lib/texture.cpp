#include <iostream>
#include <fstream>

#include "texture.h"

Texture::Texture(unsigned int width, unsigned int height) : width(width), height(height) {
  framebuffer.reserve(width * height);
  initGL();
  Update();
}

Texture::Texture(const std::string &raw, unsigned int width, unsigned int height) : width(width), height(height) {
  framebuffer.reserve(width * height * sizeof(Pixel));

  // Open file stream
  std::ifstream image_stream(raw, std::ios::binary);

  if (!image_stream.is_open())
    std::cerr << "Could not open texture " << raw << std::endl;

  // Load the
  image_stream.read((char *)framebuffer.data(), framebuffer.capacity()*sizeof(Pixel));
  image_stream.close();
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::Update() {
  Use();
  // Upload texture to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, framebuffer.data());
}

void Texture::Use() {
  glBindTexture(GL_TEXTURE_2D, texture);
}

GLuint Texture::GetTexture() {
  return texture;
}

Texture::Pixel *Texture::GetFramebuffer() {
  return framebuffer.data();
}

Texture::Pixel *Texture::GetPixel(int x, int y) {
  return &framebuffer[x+y*width];
}