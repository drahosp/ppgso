#ifndef PPGSO_TEXTURE_H
#define PPGSO_TEXTURE_H

#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>

class Texture {
public:
  struct Pixel {
    unsigned char r,g,b;
  };
  Texture(unsigned int width, unsigned int height);
  Texture(const std::string &raw, unsigned int width, unsigned int height);
  ~Texture();

  void Update();
  Pixel* GetFramebuffer();
  GLuint GetTexture();
  Pixel* GetPixel(int x, int y);
  void Use();

  unsigned int width, height;
private:
  void initGL();
  std::vector<Pixel> framebuffer;
  GLuint texture;
};
typedef std::shared_ptr< Texture > TexturePtr;

#endif //PPGSO_TEXTURE_H
