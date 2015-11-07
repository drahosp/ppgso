// Example gl_animate
// - Demonstrates the use of a dynamically generated framebuffer on the CPU
// - Displays the framebuffer as texture on a quad using OpenGL
// - Basic animation achieved by incrementing a parameter used in the image generation

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include shader sources and shader program loader
#include "shaderprogram.h"
#include "gl_animate_vert.h"
#include "gl_animate_frag.h"

const unsigned int SIZE = 512;

struct Pixel {
  unsigned char r,g,b;
};

Pixel framebuffer[SIZE][SIZE];

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

// Update framebuffer
void UpdateFramebuffer(float time) {
  // draw something to the buffer
  float cx = std::sin(time/20.0f);
  float cy = std::cos(time/19.0f);

  for(unsigned int x=0; x<SIZE; x++) {
    for(unsigned int y=0; y<SIZE; y++) {
      float fx = x/512.0f-.5f;
      float fy = y/512.0f-.5f;
      float dist = std::sqrt(std::pow(fx-cx, 2.0f) + std::pow(fy-cy, 2.0f));

      framebuffer[x][y].r = (unsigned char)(std::sin(dist*45.0f)*127+128);
      framebuffer[x][y].g = (unsigned char)(std::sin(dist*44.0f)*127+128);
      framebuffer[x][y].b = (unsigned char)(std::sin(dist*46.0f)*127+128);
    }
  }
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

  // Create shading program from included shader sources
  auto program_id = ShaderProgram(gl_animate_vert, gl_animate_frag);
  glUseProgram(program_id);

  InitializeGeometry(program_id);

  // Create texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Bind texture
  auto texture_attrib = glGetUniformLocation(program_id, "Texture");
  glUniform1i(texture_attrib, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Time counter
  float time = 0;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Update Framebuffer
    UpdateFramebuffer(time++);

    // Update texture data with framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
