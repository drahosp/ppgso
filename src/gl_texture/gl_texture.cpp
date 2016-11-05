// Example gl_texture
// - Demonstrates how to use texture as input for a shader, alternatively use ppgso::Texture for convenience
// - Texture coordinates are passed using additional vertex buffer object
// - The texture itself is loaded from raw RGB image file directly into OpenGL

#include <iostream>
#include <vector>
#include <fstream>

#include <ppgso/ppgso.h>

#include "gl_texture_vert.h"
#include "gl_texture_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class TextureWindow : public Window {
private:
  Shader program{gl_texture_vert, gl_texture_frag};
  Mesh quad{program, "quad.obj"};

  // Load a new image from a raw RGB file directly into OpenGL memory
  GLuint LoadImage(const string &image_file, unsigned int width, unsigned int height) {
    // Create new texture object
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Read raw data
    ifstream image_stream(image_file, ios::binary);

    // Setup buffer for pixels (r,g,b bytes), since we will not manipulate the image just keep it as char
    vector<char> buffer(width*height*3);
    image_stream.read(buffer.data(), buffer.size());
    image_stream.close();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

    return texture_id;
  }

public:
  TextureWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Load the image from file
    auto texture_id = LoadImage("lena.rgb", SIZE, SIZE);

    // Set the program uniform input "Texture" to the loaded data
    auto texture_attrib = program.GetUniformLocation("Texture");

    glUniform1i(texture_attrib, 0);
    glActiveTexture(GL_TEXTURE0 + 0);

    // Bind the texture for use
    glBindTexture(GL_TEXTURE_2D, texture_id);
  }

  void onPool() {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the geometry
    quad.Render();
  }
};

int main() {
  // Create our window instance
  auto window = TextureWindow{"gl_texture", SIZE, SIZE};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
