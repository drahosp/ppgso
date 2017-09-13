#include <glm/gtc/random.hpp>
#include "scene.h"
#include "projectile.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Projectile::mesh;
unique_ptr<Shader> Projectile::shader;
unique_ptr<Texture> Projectile::texture;

Projectile::Projectile() {
  // Set default speed
  speed = {0.0f, 3.0f, 0.0f};
  rotMomentum = {0.0f, 0.0f, linearRand(-PI/4.0f, PI/4.0f)};

  // Initialize static resources if needed
  if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
  if (!texture) texture = make_unique<Texture>(image::loadBMP("missile.bmp"));
  if (!mesh) mesh = make_unique<Mesh>("missile.obj");
}

bool Projectile::update(Scene &scene, float dt) {
  // Increase age
  age += dt;

  // Accelerate
  speed += vec3{0.0f, 20.0f, 0.0f} * dt;
  rotation += rotMomentum * dt;

  // Move the projectile
  position += speed * dt;

  // Die after 5s
  if (age > 5.0f) return false;

  generateModelMatrix();
  return true;
}

void Projectile::render(Scene &scene) {
  shader->use();

  // Set up light
  shader->setUniform("LightDirection", scene.lightDirection);

  // use camera
  shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
  shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

  // render mesh
  shader->setUniform("ModelMatrix", modelMatrix);
  shader->setUniform("Texture", *texture);
  mesh->render();
}

void Projectile::destroy() {
  // This will destroy the projectile on Update
  age = 100.0f;
}
