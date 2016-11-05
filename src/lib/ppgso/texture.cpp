#include <iostream>
#include <fstream>

#include "texture.h"

using namespace std;
using namespace ppgso;

/*!
 * Create new empty texture and bind it to OpenGL.
 *
 * @param width - Width in pixels.
 * @param height - Height in pixels.
 */
Texture::Texture(unsigned int width, unsigned int height) : width(width), height(height) {
  framebuffer.resize(width * height);
  initGL();
  Update();
}

/*!
 * Load raw texture into OpenGL.
 *
 * @param raw - File path to a raw RGB texture.
 * @param width - Width in pixels.
 * @param height - Height in pixels.
 */
Texture::Texture(const string &raw, unsigned int width, unsigned int height) : width(width), height(height) {
  framebuffer.resize(width * height);

  // Open file stream
  ifstream image_stream(raw, ios::binary);

  if (!image_stream.is_open())
    cerr << "Could not open texture " << raw << endl;

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

/*!
 * Update the OpenGL texture in memory.
 */
void Texture::Update() {
  Use();
  // Upload texture to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, framebuffer.data());
}

/*!
 * Bind the OpenGL texture for use.
 */
void Texture::Use() {
  glBindTexture(GL_TEXTURE_2D, texture);
}

/*!
 * Get OpenGL texture identifier number.
 *
 * @return - OpenGL texture identifier number.
 */
GLuint Texture::GetTexture() {
  return texture;
}

/*!
 * Get raw access to the texture data.
 *
 * @return - Pointer to the raw RGB framebuffer data.
 */
Texture::Pixel *Texture::GetFramebuffer() {
  return framebuffer.data();
}

/*!
 * Get single pixel from the framebuffer.
 *
 * @param x - X position of the pixel in the framebuffer.
 * @param y - Y position of the pixel in the framebuffer.
 * @return - Reference to the pixel.
 */
Texture::Pixel *Texture::GetPixel(int x, int y) {
  return &framebuffer[x+y*width];
}
