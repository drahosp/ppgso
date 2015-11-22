#include "scene.h"
#include "projectile.h"

#include "object_vert.h"
#include "object_frag.h"

Projectile::Projectile() {
  // Initialize age to 0
  age = 0;

  // Set default speed
  speed = glm::vec3(0.0f, 3.0f, 0.0f);
  rotMomentum = glm::vec3(0.0f, 0.0f, Rand(-PI/4.0f, PI/4.0f));

  // Initialize static resources if needed
  if (!shader) shader = ShaderPtr(new Shader{object_vert, object_frag});
  if (!texture) texture = TexturePtr(new Texture{"missile.rgb", 512, 512});
  if (!mesh) mesh = MeshPtr(new Mesh{shader, "missile.obj"});
}

Projectile::~Projectile() {
}

bool Projectile::Update(Scene &scene, float dt) {
  // Increase age
  age += dt;

  // Accelerate
  speed += glm::vec3(0.0f, 0.2f, 0.0f);
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

ShaderPtr Projectile::shader;
MeshPtr Projectile::mesh;
TexturePtr Projectile::texture;

void Projectile::Destroy() {
  // This will destroy the projectile on Update
  age = 100.0f;
}
