#include "scene.h"
#include "projectile.h"

#include "object_vert.h"
#include "object_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
shared_ptr<Mesh> Projectile::mesh;
shared_ptr<Shader> Projectile::shader;
shared_ptr<Texture> Projectile::texture;

Projectile::Projectile() {
  // Set default speed
  speed = {0.0f, 3.0f, 0.0f};
  rotMomentum = {0.0f, 0.0f, Rand(-PI/4.0f, PI/4.0f)};

  // Initialize static resources if needed
  if (!shader) shader = make_shared<Shader>(object_vert, object_frag);
  if (!texture) texture = make_shared<Texture>("missile.rgb", 512, 512);
  if (!mesh) mesh = make_shared<Mesh>(shader, "missile.obj");
}

Projectile::~Projectile() {
}

bool Projectile::Update(Scene &scene, float dt) {
  // Increase age
  age += dt;

  // Accelerate
  speed += vec3{0.0f, 0.2f, 0.0f};
  rotation += rotMomentum;

  // Move the projectile
  position += speed * dt;

  // Die after 5s
  if (age > 5.0f) return false;

  GenerateModelMatrix();
  return true;
}

void Projectile::Render(Scene &scene) {
  shader->Use();

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}

void Projectile::Destroy() {
  // This will destroy the projectile on Update
  age = 100.0f;
}
