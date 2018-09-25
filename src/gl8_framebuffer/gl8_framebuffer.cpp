// Example gl_framebuffer
// - Demonstrates use of Framebuffer Object (FBO)
// - Renders a scene to a texture in graphics memory and uses this texture in the final scene displayed on screen

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include <shaders/convolution_vert_glsl.h>
#include <shaders/convolution_frag_glsl.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

/*!
 * Custom window for displaying FBO object
 */
class FramebufferWindow : public Window {
private:
  // Objects to be rendered into the framebuffer
  Shader sphereShader = {texture_vert_glsl, texture_frag_glsl};
  Mesh sphereMesh = {"sphere.obj"};
  Texture sphereTexture = {image::loadBMP("sphere.bmp")};

  // Objects to render the framebuffer on to
  Shader quadShader = {convolution_vert_glsl, convolution_frag_glsl};
  Mesh quadMesh = {"quad.obj"};
  Texture quadTexture = {SIZE, SIZE};

  // OpenGL object ids for framebuffer and render buffer
  GLuint fbo = 0;
  GLuint rbo = 0;
public:
  /*!
   * Constructor for our custom window
   */
  FramebufferWindow() : Window{"gl8_framebuffer", SIZE, SIZE} {
    // Set up OpenGL options
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Disable mipmapping on the quadTexture
    quadTexture.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Initialize framebuffer, its color texture (the sphere will be rendered to it) and its render buffer for depth info storage
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Set up render buffer that has a depth buffer and stencil buffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Associate the quadTexture with it
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, quadTexture.image.width, quadTexture.image.height);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, quadTexture.getTexture(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw runtime_error("Cannot create framebuffer!");
    }
  }

  /*!
   * Free any allocated OpenGL resources
   */
  ~FramebufferWindow() override {
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
  }

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    auto time = (float) glfwGetTime();

    // --------
    // Pass 1 - Render a scene with sphere to a texture in graphics memory
    // --------
    // Set rendering target to texture
    glViewport(0, 0, SIZE, SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Clear the framebuffer
    glClearColor(.5f, .7f, .5f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
    auto sphereProjectionMatrix = perspective((PI / 180.f) * 60.0f, (float)SIZE/(float)SIZE, 1.0f, 10.0f);

    // Create view matrix (translate camera backwards a bit, so we can see the geometry)
    auto sphereViewMatrix = translate(mat4{1.0}, {0.0f, 0.0f, -1.5f});

    // Assign sphere texture
    auto sphereModelMatrix = rotate(mat4{1.0f}, time, {0, 1, 0});

    // Set shader inputs
    sphereShader.use();
    sphereShader.setUniform("ProjectionMatrix", sphereProjectionMatrix);
    sphereShader.setUniform("ViewMatrix", sphereViewMatrix);
    sphereShader.setUniform("ModelMatrix", sphereModelMatrix);
    sphereShader.setUniform("Texture", sphereTexture);
    sphereMesh.render();

    // --------
    // Pass 2 - Render the final scene to screen
    // --------
    // Set rendering target to screen
    resetViewport();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear the framebuffer
    glClearColor(.2f, .2f, .2f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
    auto quadProjectionMatrix = perspective((PI / 180.f) * 60.0f, (float)SIZE/(float)SIZE, 1.0f, 10.0f);

    // Create view matrix (translate camera backwards a bit, so we can see the geometry)
    auto quadViewMatrix = translate(mat4{1.0f}, {0.0f, 0.0f, -3.0f});

    // Animate rotation of the quad
    auto quadModelMatrix = rotate(mat4{1.0f}, sin(time / 2.0f) * 1.5f, {0, 1, .1});

    // Set shader inputs
    quadShader.use();
    quadShader.setUniform("ProjectionMatrix", quadProjectionMatrix);
    quadShader.setUniform("ViewMatrix", quadViewMatrix);
    quadShader.setUniform("ModelMatrix", quadModelMatrix);
    quadShader.setUniform("Texture", quadTexture);
    quadMesh.render();
  }
};

int main() {
  // Create our custom window
  FramebufferWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
