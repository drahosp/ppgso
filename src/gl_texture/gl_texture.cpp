// Example gl_texture
// - Demonstrates how to use texture as input for a shader
// - Texture coordinates are passed using additional vertex buffer object
// - The texture itself is loaded from raw RGB image file directly into OpenGL

#include <iostream>
#include <vector>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "mesh.h"
#include "gl_texture_vert.h"
#include "gl_texture_frag.h"

const unsigned int SIZE = 512;

// Load a new image from a raw RGB file directly into OpenGL memory
GLuint LoadImage(const std::string &image_file, unsigned int width, unsigned int height) {
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
  std::ifstream image_stream(image_file, std::ios::binary);

  // Setup buffer for pixels (r,g,b bytes), since we will not manipulate the image just keep it as char
  std::vector<char> buffer(width*height*3);
  image_stream.read(buffer.data(), buffer.size());
  image_stream.close();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

  return texture_id;
}

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    return EXIT_FAILURE;
  }

  // Setup OpenGL context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Try to create a window
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_texture", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Finalize window setup
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_3")) {
    std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Load shaders
  auto program = ShaderPtr(new Shader{gl_texture_vert, gl_texture_frag});
  program->Use();

  auto quad = Mesh{program, "quad.obj"};

  // Load and bind texture
  auto texture_id = LoadImage("lena.rgb", SIZE, SIZE);
  auto texture_attrib = program->GetUniformLocation("Texture");
  glUniform1i(texture_attrib, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quad.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
