#ifndef PPGSO_TGA_H
#define PPGSO_TGA_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#pragma pack(push, 1)
struct TGA_Header {
  char idlength;
  char colormaptype;
  char datatypecode;
  short colormaporigin;
  short colormaplength;
  char colormapdepth;
  short x_origin;
  short y_origin;
  short width;
  short height;
  char bitsperpixel;
  char imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {
  unsigned char bgra[4];
  unsigned char bytespp;

  TGAColor();

  TGAColor(unsigned char R, unsigned char G, unsigned char B,
           unsigned char A = 255);

  TGAColor(unsigned char v);

  TGAColor(const unsigned char *p, unsigned char bpp);

  unsigned char &operator[](const int i);

  TGAColor operator*(float intensity) const;
};

class TGAImage {
protected:
  unsigned char *data;
  int width;
  int height;
  int bytespp;

  bool load_rle_data(std::ifstream &in);

  // TODO: it is not necessary to break a raw chunk for two equal pixels (for
  // the matter of the resulting size)
  bool unload_rle_data(std::ofstream &out);

public:
  enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

  TGAImage();

  TGAImage(int w, int h, int bpp, void *image = NULL);

  TGAImage(const TGAImage &img);

  ~TGAImage();

  TGAImage &operator=(const TGAImage &img);

  bool read_tga_file(const std::string &filename);

  bool write_tga_file(const std::string &filename, bool rle);

  TGAColor get(int x, int y);

  bool set(int x, int y, TGAColor &c);

  bool set(int x, int y, const TGAColor &c);

  int get_bytespp();

  int get_width();

  int get_height();

  bool flip_horizontally();

  bool flip_vertically();

  unsigned char *buffer();

  void clear();

  bool scale(int w, int h);
};

#endif // PPGSO_TGA_H
