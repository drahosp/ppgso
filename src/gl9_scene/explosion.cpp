#include <glm/gtc/random.hpp>
#include "scene.h"
#include "explosion.h"

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// static resources
unique_ptr<Mesh> Explosion::mesh;
unique_ptr<Texture> Explosion::texture;
unique_ptr<Shader> Explosion::shader;

Explosion::Explosion() {
  // Random rotation and momentum
  rotation = ballRand(PI)*3.0f;
  rotMomentum = ballRand(PI)*3.0f;
  speed = {0.0f, 0.0f, 0.0f};

  // Initialize static resources if needed
  if (!shader) shader = make_unique<Shader>(texture_vert_glsl, texture_frag_glsl);
  if (!texture) texture = make_unique<Texture>(image::loadBMP("explosion.bmp"));
  if (!mesh) mesh = make_unique<Mesh>("asteroid.obj");
}

void Explosion::render(Scene &scene) {
  shader->use();

  // Transparency, interpolate from 1.0f -> 0.0f
  shader->setUniform("Transparency", 1.0f - age / maxAge);

  // use camera
  shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
  shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

  // render mesh
  shader->setUniform("ModelMatrix", modelMatrix);
  shader->setUniform("Texture", *texture);

  // Disable depth testing
  glDisable(GL_DEPTH_TEST);

  // Enable blending
  glEnable(GL_BLEND);
  // Additive blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  mesh->render();

  // Disable blending
  glDisable(GL_BLEND);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
}

bool Explosion::update(Scene &scene, float dt) {
  // Update scale and rotation
  scale = scale * ( 1.0f + dt * 5.0f);
  rotation += rotMomentum * dt;
  position += speed * dt;

  // Die after reaching maxAge
  age += dt;
  if (age > maxAge) return false;

  generateModelMatrix();
  return true;
}
