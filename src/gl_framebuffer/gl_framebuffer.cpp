// Example gl_framebuffer
// - Demonstrates use of Framebuffer Object (FBO)
// - Renders a scene to a texture in graphics memory and uses this texture in the final scene displayed on screen

#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "mesh.h"

#include "gl_framebuffer_vert.h"
#include "gl_framebuffer_frag.h"
#include "texture_vert.h"
#include "texture_frag.h"

const unsigned int SIZE = 512;

#define PI 3.14159265358979323846f

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
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_framebuffer", nullptr, nullptr);
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
  glGetError();

  // Set up OpenGL options
  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // --------
  // Common initialization
  // --------
  // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
  auto projection = glm::perspective( (PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);


  // --------
  // Part 1 initialization - Rendering a scene with sphere to a texture in graphics memory
  // --------
  // Create shading program
  auto program1 = ShaderPtr(new Shader{texture_vert, texture_frag});
  program1->Use();

  // Load a sphere mesh
  auto sphere = Mesh{program1, "sphere.obj"};

  // Initialize texture for sphere mesh
  auto sphereTexture = TexturePtr(new Texture{"sphere.rgb", 256, 256});
  
  // Initialize framebuffer, its color texture (the sphere will be rendered to it) and its renderbuffer for depth info storage
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  auto fboTexture = TexturePtr(new Texture{SIZE, SIZE});
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture->GetTexture(), 0);

  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SIZE, SIZE);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Cannot create framebuffer!" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Set shader values
  program1->SetMatrix(projection, "ProjectionMatrix");
  // Create view matrix (translate camera backwards a bit, so we can see the geometry)
  auto view1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f));
  program1->SetMatrix(view1, "ViewMatrix");
  program1->SetMatrix(glm::mat4(1.0f), "ModelMatrix");


  // --------
  // Part 2 initialization - Rendering a final scene with quad to screen
  // --------
  // Create shading program
  auto program2 = ShaderPtr(new Shader{gl_framebuffer_vert, gl_framebuffer_frag});
  program2->Use();

  // Load a quad mesh
  auto quad = Mesh{program2, "quad.obj"};

  // Set shader values
  program2->SetMatrix(projection, "ProjectionMatrix");
  // Create view matrix (translate camera backwards a bit, so we can see the geometry)
  auto view2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  program2->SetMatrix(view2, "ViewMatrix");


  // Time counter
  float time;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    time = (float)glfwGetTime();

    // --------
    // Part 1 - Render a scene with sphere to a texture in graphics memory
    // --------
    // Set rendering target to texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Clear the framebuffer
    glClearColor(.5f,.7f,.5f,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program1->Use();

    // Assign sphere texture
    program1->SetTexture(sphereTexture, "Texture");

    sphere.Render();

    // --------
    // Part 2 - Render the final scene to screen
    // --------
    // Set rendering target to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear the framebuffer
    glClearColor(.2f,.2f,.2f,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program2->Use();

    // Assign framebuffer texture as quad texture
    program2->SetTexture(fboTexture, "Texture");

    // Animate rotation of the quad
    auto model2 = glm::rotate(glm::mat4(1.0f), ((float) sin(time / 2.0f)) * 1.5f, glm::vec3(0,1,0));
    program2->SetMatrix(model2, "ModelMatrix");

    quad.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
