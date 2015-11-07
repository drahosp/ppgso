// Example gl_transform
// - This example demonstrates basic 2D transformations
// - Notice that the glm library uses column major order when declaring matrices
// - Try to combilne multiple transformation using multiplication

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderprogram.h"
#include "gl_transform_vert.h"
#include "gl_transform_frag.h"

const unsigned int SIZE = 512;

void InitializeGeometry(GLuint program_id) {
  // Generate a vertex array object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Setup geometry
  std::vector<GLfloat> vertex_buffer {
    // x, y
    1.0f, 1.0f,
      -1.0f, 1.0f,
      1.0f, -1.0f,
      -1.0f, -1.0f
  };

  // Generate a vertex buffer object
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

  // Setup vertex array lookup
  auto position_attrib = glGetAttribLocation(program_id, "Position");
  glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attrib);

  // Generate another vertex buffer object for texture coordinates
  std::vector<GLfloat> texcoord_buffer {
    // u, v
    1.0f, 0.0f,
      0.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f
  };

  GLuint tbo;
  glGenBuffers(1, &tbo);
  glBindBuffer(GL_ARRAY_BUFFER, tbo);
  glBufferData(GL_ARRAY_BUFFER, texcoord_buffer.size() * sizeof(GLfloat), texcoord_buffer.data(), GL_STATIC_DRAW);

  auto texcoord_attrib = glGetAttribLocation(program_id, "TexCoord");
  glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(texcoord_attrib);
}

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

void SetTransformation(GLuint program_id, float time) {
  // Create transformation matrix
  // NOTE: glm matrices are declared column major !

  // identity
  auto transform = glm::mat3({
     1.0, 0.0, 0.0,
     0.0, 1.0, 0.0,
     0.0, 0.0, 1.0
  });

  // scale
  // transform *= glm::mat3({
  //    std::sin(time), 0.0, 0.0,
  //    0.0, std::sin(time), 0.0,
  //    0.0, 0.0, 1.0
  //  });

  // squash
  // transform *= glm::mat3({
  //   std::sin(time), 0.0, 0.0,
  //   0.0, std::cos(time), 0.0,
  //   0.0, 0.0, 1.0
  // });

  // rotate
  transform *= glm::mat3({
     std::cos(time), std::sin(time), 0.0,
     -std::sin(time), std::cos(time), 0.0,
     0.0, 0.0, 1.0
  });

  // translate
  // transform *= glm::mat3({
  //    1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   std::sin(time)/2.0, std::cos(time)/2.0, 1.0
  // });

  // Rotate around top right corner
  // transform *= glm::mat3({
  //   // Move to origin
  //   1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   1.0, 1.0, 1.0
  // }) * glm::mat3({
  //   // Rotate
  //   std::cos(time), std::sin(time), 0.0,
  //   -std::sin(time), std::cos(time), 0.0,
  //   0.0, 0.0, 1.0
  // }) * glm::mat3({
  //   // Move back
  //   1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   -1.0, -1.0, 1.0
  // });

  // Send matrix value to program
  auto transform_uniform = glGetUniformLocation(program_id, "Transform");
  glUniformMatrix3fv(transform_uniform, 1, GL_FALSE, glm::value_ptr(transform));
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
  auto window = glfwCreateWindow( SIZE, SIZE, "OpenGL", NULL, NULL);
  if (window == NULL) {
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
  auto program_id = ShaderProgram(gl_transform_vert, gl_transform_frag);
  glUseProgram(program_id);

  InitializeGeometry(program_id);

  // Load and bind texture
  auto texture_id = LoadImage("lena.rgb", SIZE, SIZE);
  auto texture_attrib = glGetUniformLocation(program_id, "Texture");
  glUniform1i(texture_attrib, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  float time = 0;
  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetTransformation(program_id, time+=0.01);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
