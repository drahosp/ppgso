#ifndef PPGSO_TEXTURE_H
#define PPGSO_TEXTURE_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include <GL/glew.h>

namespace ppgso {

  class Texture {
    // Structs for reading/writing BMP files
    #pragma pack(2)
    typedef struct                     /**** BMP file header structure ****/
    {
      unsigned short bfType;           /* Magic number for file */
      unsigned int   bfSize;           /* Size of file */
      unsigned short bfReserved1;      /* Reserved */
      unsigned short bfReserved2;      /* ... */
      unsigned int   bfOffBits;        /* Offset to bitmap data */
    } BITMAPFILEHEADER;

    typedef struct                     /**** BMP file info structure ****/
    {
      unsigned int   biSize;           /* Size of info header */
      int            biWidth;          /* Width of image */
      int            biHeight;         /* Height of image */
      unsigned short biPlanes;         /* Number of color planes */
      unsigned short biBitCount;       /* Number of bits per pixel */
      unsigned int   biCompression;    /* Type of compression to use */
      unsigned int   biSizeImage;      /* Size of image data */
      int            biXPelsPerMeter;  /* X pixels per meter */
      int            biYPelsPerMeter;  /* Y pixels per meter */
      unsigned int   biClrUsed;        /* Number of colors used */
      unsigned int   biClrImportant;   /* Number of important colors */
    } BITMAPINFOHEADER;
    #pragma pack()

  public:
    struct Pixel {
      unsigned char r, g, b;
    };

    /*!
     * Create new empty texture and bind it to OpenGL.
     *
     * @param width - Width in pixels.
     * @param height - Height in pixels.
     */
    Texture(unsigned int width, unsigned int height);

    /*!
     * Load raw texture into OpenGL.
     *
     * @param raw - File path to a raw RGB texture.
     * @param width - Width in pixels.
     * @param height - Height in pixels.
     */
    Texture(const std::string &raw, unsigned int width, unsigned int height);

    /*!
     * Load BMP image as texture and bind it into OpenGL. Only uncompressed RGB format is supported.
     *
     * @param bmp - File path to a BMP image.
     */
    Texture(const std::string &bmp);

    ~Texture();

    void Save(const std::string &bmp);

    /*!
     * Update the OpenGL texture in memory.
     */
    void Update();

    /*!
     * Get raw access to the texture data.
     *
     * @return - Pointer to the raw RGB framebuffer data.
     */
    Pixel *Data();

    /*!
     * Get OpenGL texture identifier number.
     *
     * @return - OpenGL texture identifier number.
     */
    GLuint GetTexture();

    /*!
     * Get single pixel from the framebuffer.
     *
     * @param x - X position of the pixel in the framebuffer.
     * @param y - Y position of the pixel in the framebuffer.
     * @return - Reference to the pixel.
     */
    Pixel *GetPixel(int x, int y);

    /*!
     * Bind the OpenGL texture for use.
     */
    void Use();

    unsigned int width, height;
  private:
    void initGL();
    void load(const std::string &bmp);
    void load(const std::string &raw, unsigned int width, unsigned int height);

    std::vector<Pixel> framebuffer;
    GLuint texture;
  };
}

#endif //PPGSO_TEXTURE_H
