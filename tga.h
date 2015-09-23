#ifndef TGAIMAGE_CLASS
#define TGAIMAGE_CLASS

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#pragma pack(push,1)
struct TGA_Header {
  char  idlength;
  char  colormaptype;
  char  datatypecode;
  short colormaporigin;
  short colormaplength;
  char  colormapdepth;
  short x_origin;
  short y_origin;
  short width;
  short height;
  char  bitsperpixel;
  char  imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {
  unsigned char bgra[4];
  unsigned char bytespp;

  TGAColor() : bgra(), bytespp(1) {
    for (int i = 0; i < 4; i++) bgra[i] = 0;
  }

  TGAColor(unsigned char R,
           unsigned char G,
           unsigned char B,
           unsigned char A = 255) : bgra(), bytespp(4) {
    bgra[0] = B;
    bgra[1] = G;
    bgra[2] = R;
    bgra[3] = A;
  }

  TGAColor(unsigned char v) : bgra(), bytespp(1) {
    for (int i = 0; i < 4; i++) bgra[i] = 0;
    bgra[0] = v;
  }


  TGAColor(const unsigned char *p, unsigned char bpp) : bgra(), bytespp(bpp) {
    for (int i = 0; i < (int)bpp; i++) {
      bgra[i] = p[i];
    }

    for (int i = bpp; i < 4; i++) {
      bgra[i] = 0;
    }
  }

  unsigned char& operator[] (const int i) {return bgra[i]; }

  TGAColor operator *(float intensity)const {
    TGAColor res = *this;
    intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));

    for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
    return res;
  }
};

class TGAImage {
protected:

  unsigned char *data;
  int width;
  int height;
  int bytespp;

  bool load_rle_data(std::ifstream& in) {
    unsigned long pixelcount   = width * height;
    unsigned long currentpixel = 0;
    unsigned long currentbyte  = 0;
    TGAColor colorbuffer;

    do {
      unsigned char chunkheader = 0;
      chunkheader = in.get();

      if (!in.good()) {
        std::cerr << "an error occured while reading the data\n";
        return false;
      }

      if (chunkheader < 128) {
        chunkheader++;

        for (int i = 0; i < chunkheader; i++) {
          in.read((char *)colorbuffer.bgra, bytespp);

          if (!in.good()) {
            std::cerr << "an error occured while reading the header\n";
            return false;
          }

          for (int t = 0; t < bytespp;
               t++) data[currentbyte++] = colorbuffer.bgra[t];
          currentpixel++;

          if (currentpixel > pixelcount) {
            std::cerr << "Too many pixels read\n";
            return false;
          }
        }
      } else {
        chunkheader -= 127;
        in.read((char *)colorbuffer.bgra, bytespp);

        if (!in.good()) {
          std::cerr << "an error occured while reading the header\n";
          return false;
        }

        for (int i = 0; i < chunkheader; i++) {
          for (int t = 0; t < bytespp;
               t++) data[currentbyte++] = colorbuffer.bgra[t];
          currentpixel++;

          if (currentpixel > pixelcount) {
            std::cerr << "Too many pixels read\n";
            return false;
          }
        }
      }
    } while (currentpixel < pixelcount);
    return true;
  }

  // TODO: it is not necessary to break a raw chunk for two equal pixels (for
  // the matter of the resulting size)
  bool unload_rle_data(std::ofstream& out) {
    const unsigned char max_chunk_length = 128;
    unsigned long npixels                = width * height;
    unsigned long curpix                 = 0;

    while (curpix < npixels) {
      unsigned long chunkstart = curpix * bytespp;
      unsigned long curbyte    = curpix * bytespp;
      unsigned char run_length = 1;
      bool raw                 = true;

      while (curpix + run_length < npixels && run_length < max_chunk_length) {
        bool succ_eq = true;

        for (int t = 0; succ_eq && t < bytespp; t++) {
          succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);
        }
        curbyte += bytespp;

        if (1 == run_length) {
          raw = !succ_eq;
        }

        if (raw && succ_eq) {
          run_length--;
          break;
        }

        if (!raw && !succ_eq) {
          break;
        }
        run_length++;
      }
      curpix += run_length;
      out.put(raw ? run_length - 1 : run_length + 127);

      if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        return false;
      }
      out.write((char *)(data + chunkstart),
                (raw ? run_length * bytespp : bytespp));

      if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        return false;
      }
    }
    return true;
  }

public:

  enum Format {
    GRAYSCALE = 1, RGB = 3, RGBA = 4
  };

  TGAImage() : data(NULL), width(0), height(0), bytespp(0) {}

  TGAImage(int w, int h, int bpp, void *image=NULL) : width(w), height(h),
                                    bytespp(bpp) {
    unsigned long nbytes = width * height * bytespp;
    
    data = new unsigned char[nbytes];
    
    if (image == NULL ) {
      std::memset(data, 0, nbytes);
    } else {
      std::memcpy(data, image, nbytes);

      // swap RGB into BRG
      for (unsigned long i = 0; i < nbytes; i+=3)
        std::swap(data[i], data[i+2]); 
    }
  }

  TGAImage(const TGAImage &img) : data(NULL), width(img.width),
                                  height(img.height), bytespp(img.bytespp) {
    unsigned long nbytes = width * height * bytespp;

    data = new unsigned char[nbytes];
    memcpy(data, img.data, nbytes);
  }

  ~TGAImage() {
    if (data) delete[] data;
  }

  TGAImage& operator = (const TGAImage &img) {
    if (this != &img) {
      if (data) delete[] data;
      width   = img.width;
      height  = img.height;
      bytespp = img.bytespp;
      unsigned long nbytes = width * height * bytespp;
      data = new unsigned char[nbytes];
      memcpy(data, img.data, nbytes);
    }
    return *this;
  }

  bool read_tga_file(const std::string &filename) {
    if (data) delete[] data;
    data = NULL;
    std::ifstream in;
    in.open(filename, std::ios::binary);

    if (!in.is_open()) {
      std::cerr << "can't open file " << filename << "\n";
      in.close();
      return false;
    }
    TGA_Header header;
    in.read((char *)&header, sizeof(header));

    if (!in.good()) {
      in.close();
      std::cerr << "an error occured while reading the header\n";
      return false;
    }
    width   = header.width;
    height  = header.height;
    bytespp = header.bitsperpixel >> 3;

    if ((width <= 0) || (height <= 0) ||
        ((bytespp != GRAYSCALE) && (bytespp != RGB) && (bytespp != RGBA))) {
      in.close();
      std::cerr << "bad bpp (or width/height) value\n";
      return false;
    }
    unsigned long nbytes = bytespp * width * height;
    data = new unsigned char[nbytes];

    if ((3 == header.datatypecode) || (2 == header.datatypecode)) {
      in.read((char *)data, nbytes);

      if (!in.good()) {
        in.close();
        std::cerr << "an error occured while reading the data\n";
        return false;
      }
    } else if ((10 == header.datatypecode) || (11 == header.datatypecode)) {
      if (!load_rle_data(in)) {
        in.close();
        std::cerr << "an error occured while reading the data\n";
        return false;
      }
    } else {
      in.close();
      std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
      return false;
    }

    if (!(header.imagedescriptor & 0x20)) {
      flip_vertically();
    }

    if (header.imagedescriptor & 0x10) {
      flip_horizontally();
    }
    std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";
    in.close();
    return true;
  }

  bool write_tga_file(const std::string &filename, bool rle) {
    unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };
    unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };
    unsigned char footer[18]            =
    { 'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L',
      'E', '.', '\0' };
    std::ofstream out;

    out.open(filename, std::ios::binary);

    if (!out.is_open()) {
      std::cerr << "can't open file " << filename << "\n";
      out.close();
      return false;
    }
    TGA_Header header;
    memset((void *)&header, 0, sizeof(header));
    header.bitsperpixel = bytespp << 3;
    header.width        = width;
    header.height       = height;
    header.datatypecode =
      (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imagedescriptor = 0x20; // top-left origin
    out.write((char *)&header, sizeof(header));

    if (!out.good()) {
      out.close();
      std::cerr << "can't dump the tga file\n";
      return false;
    }

    if (!rle) {
      out.write((char *)data, width * height * bytespp);

      if (!out.good()) {
        std::cerr << "can't unload raw data\n";
        out.close();
        return false;
      }
    } else {
      if (!unload_rle_data(out)) {
        out.close();
        std::cerr << "can't unload rle data\n";
        return false;
      }
    }
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));

    if (!out.good()) {
      std::cerr << "can't dump the tga file\n";
      out.close();
      return false;
    }
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));

    if (!out.good()) {
      std::cerr << "can't dump the tga file\n";
      out.close();
      return false;
    }
    out.write((char *)footer, sizeof(footer));

    if (!out.good()) {
      std::cerr << "can't dump the tga file\n";
      out.close();
      return false;
    }
    out.close();
    return true;
  }

  TGAColor get(int x, int y) {
    if (!data || (x < 0) || (y < 0) || (x >= width) || (y >= height)) {
      return TGAColor();
    }
    return TGAColor(data + (x + y * width) * bytespp, bytespp);
  }

  bool set(int x, int y, TGAColor& c) {
    if (!data || (x < 0) || (y < 0) || (x >= width) || (y >= height)) {
      return false;
    }
    memcpy(data + (x + y * width) * bytespp, c.bgra, bytespp);
    return true;
  }

  bool set(int x, int y, const TGAColor& c) {
    if (!data || (x < 0) || (y < 0) || (x >= width) || (y >= height)) {
      return false;
    }
    memcpy(data + (x + y * width) * bytespp, c.bgra, bytespp);
    return true;
  }

  int get_bytespp() {
    return bytespp;
  }

  int get_width() {
    return width;
  }

  int get_height() {
    return height;
  }

  bool flip_horizontally() {
    if (!data) return false;

    int half = width >> 1;

    for (int i = 0; i < half; i++) {
      for (int j = 0; j < height; j++) {
        TGAColor c1 = get(i, j);
        TGAColor c2 = get(width - 1 - i, j);
        set(i,             j, c2);
        set(width - 1 - i, j, c1);
      }
    }
    return true;
  }

  bool flip_vertically() {
    if (!data) return false;

    unsigned long  bytes_per_line = width * bytespp;
    unsigned char *line           = new unsigned char[bytes_per_line];
    int half                      = height >> 1;

    for (int j = 0; j < half; j++) {
      unsigned long l1 = j * bytes_per_line;
      unsigned long l2 = (height - 1 - j) * bytes_per_line;
      memmove((void *)line,        (void *)(data + l1), bytes_per_line);
      memmove((void *)(data + l1), (void *)(data + l2), bytes_per_line);
      memmove((void *)(data + l2), (void *)line,        bytes_per_line);
    }
    delete[] line;
    return true;
  }

  unsigned char* buffer() {
    return data;
  }

  void clear() {
    memset((void *)data, 0, width * height * bytespp);
  }

  bool scale(int w, int h) {
    if ((w <= 0) || (h <= 0) || !data) return false;

    unsigned char *tdata     = new unsigned char[w * h * bytespp];
    int nscanline            = 0;
    int oscanline            = 0;
    int erry                 = 0;
    unsigned long nlinebytes = w * bytespp;
    unsigned long olinebytes = width * bytespp;

    for (int j = 0; j < height; j++) {
      int errx = width - w;
      int nx   = -bytespp;
      int ox   = -bytespp;

      for (int i = 0; i < width; i++) {
        ox   += bytespp;
        errx += w;

        while (errx >= (int)width) {
          errx -= width;
          nx   += bytespp;
          memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);
        }
      }
      erry      += h;
      oscanline += olinebytes;

      while (erry >= (int)height) {
        if (erry >= (int)height << 1) // it means we jump over a scanline
          memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
        erry      -= height;
        nscanline += nlinebytes;
      }
    }
    delete[] data;
    data   = tdata;
    width  = w;
    height = h;
    return true;
  }
};

#endif // TGAIMAGE_CLASS
