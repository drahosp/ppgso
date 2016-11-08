// Example gl_framebuffer
// - Demonstrates use of Framebuffer Object (FBO)
// - Renders a scene to a texture in graphics memory and uses this texture in the final scene displayed on screen

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include "gl8_framebuffer_vert.h"
#include "gl8_framebuffer_frag.h"
#include "gl8_framebuffer_texture_vert.h"
#include "gl8_framebuffer_texture_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class FramebufferWindow : public Window {
private:
  // Objects to be rendered into the framebuffer
  Shader program1 = {gl8_framebuffer_texture_vert, gl8_framebuffer_texture_frag};
  Mesh object1 = {program1, "sphere.obj"};
  Texture texture1 = {"sphere.bmp"};

  // Objects to render the framebuffer on to
  Shader program2 = {gl8_framebuffer_vert, gl8_framebuffer_frag};
  Mesh object2 = {program2, "quad.obj"};
  Texture framebuffer = {SIZE, SIZE};

  GLuint fbo, rbo;
public:
  FramebufferWindow() : Window{"gl8_framebuffer", SIZE, SIZE} {
    // Set up OpenGL options
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Initialize framebuffer, its color texture (the sphere will be rendered to it) and its render buffer for depth info storage
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SIZE, SIZE);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      cerr << "Cannot create framebuffer!" << endl;
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, framebuffer.GetTexture(), 0);

    // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
    auto projection = perspective( (PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);

    // Create view matrix (translate camera backwards a bit, so we can see the geometry)
    auto view1 = translate(mat4(), {0.0f, 0.0f, -1.5f});

    // Set shader inputs
    program1.SetMatrix(projection, "ProjectionMatrix");
    program1.SetMatrix(view1, "ViewMatrix");

    // Create view matrix (translate camera backwards a bit, so we can see the geometry)
    auto view2 = translate(mat4(), {0.0f, 0.0f, -3.0f});

    // Set shader inputs
    program2.SetMatrix(projection, "ProjectionMatrix");
    program2.SetMatrix(view2, "ViewMatrix");
  }

  ~FramebufferWindow() override {
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
  }

  void onIdle() override {
    float time = (float)glfwGetTime();

    // --------
    // Part 1 - Render a scene with sphere to a texture in graphics memory
    // --------
    // Set rendering target to texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0,0,SIZE,SIZE);

    // Clear the framebuffer
    glClearColor(.5f,.7f,.5f,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Assign sphere texture
    auto model1 = rotate(mat4{}, time, {0,1,0});
    program1.Use();
    program1.SetTexture(texture1, "Texture");
    program1.SetMatrix(model1, "ModelMatrix");
    object1.Render();

    // --------
    // Part 2 - Render the final scene to screen
    // --------
    // Set rendering target to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ResetViewport();

    // Clear the framebuffer
    glClearColor(.2f,.2f,.2f,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Animate rotation of the quad
    auto model2 = rotate(mat4{}, sin(time / 2.0f) * 1.5f, {0, 1, .1});
    program2.Use();
    program2.SetTexture(framebuffer, "Texture");
    program2.SetMatrix(model2, "ModelMatrix");
    object2.Render();
  }
};

int main() {
  // Create a window with OpenGL 3.3 enabled
  auto window = FramebufferWindow{};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
