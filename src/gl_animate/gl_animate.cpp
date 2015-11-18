// Example gl_animate
// - Demonstrates the use of a dynamically generated framebuffer on the CPU
// - Displays the framebuffer as texture on a quad using OpenGL
// - Basic animation achieved by incrementing a parameter used in the image generation

#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "mesh.h"

#include "gl_animate_vert.h"
#include "gl_animate_frag.h"

const unsigned int SIZE = 512;

// Update texture framebuffer
void UpdateTexture(TexturePtr texture, float time) {
  // draw something to the buffer
  float cx = std::sin(time);
  float cy = std::cos(time*0.9f);

  for(unsigned int x = 0; x < texture->width; x++) {
    for(unsigned int y = 0; y < texture->height; y++) {
      auto pixel = texture->GetPixel(x, y);
      float fx = (float)x / (float)(texture->width) - .5f;
      float fy = (float)y / (float)(texture->height) - .5f;
      float dist = std::sqrt(std::pow(fx - cx, 2.0f) + std::pow(fy - cy, 2.0f));

      pixel->r = (unsigned char) (std::sin(dist * 45.0f) * 127 + 128);
      pixel->g = (unsigned char) (std::sin(dist * 44.0f) * 127 + 128);
      pixel->b = (unsigned char) (std::sin(dist * 46.0f) * 127 + 128);
    }
  }

  texture->Update();
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
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_animate", NULL, NULL);
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
  auto program = ShaderPtr(new Shader{gl_animate_vert, gl_animate_frag});
  program->Use();

  // Load a quad mesh
  auto quad = Mesh{program, "quad.obj"};

  // Initialize texture
  auto texture = TexturePtr(new Texture{SIZE, SIZE});
  program->SetTexture(texture, "Texture");

  // Time counter
  float time;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Update Framebuffer
    time = (float)glfwGetTime();
    UpdateTexture(texture, time);

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
