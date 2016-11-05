#include "asteroid.h"
#include "projectile.h"
#include "explosion.h"

#include "object_vert.h"
#include "object_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

// static resources
shared_ptr<Mesh> Asteroid::mesh;
shared_ptr<Texture> Asteroid::texture;
shared_ptr<Shader> Asteroid::shader;

Asteroid::Asteroid() {
  // Set random scale speed and rotation
  scale *= Rand(1.0f, 3.0f);
  speed = {Rand(-2.0f, 2.0f), Rand(-5.0f, -10.0f), 0.0f};
  rotation = {Rand(-PI, PI), Rand(-PI, PI), Rand(-PI, PI)};
  rotMomentum = {Rand(-PI, PI), Rand(-PI, PI), Rand(-PI, PI)};

  // Initialize static resources if needed
  if (!shader) shader = make_shared<Shader>(object_vert, object_frag);
  if (!texture) texture = make_shared<Texture>("asteroid.rgb", 512, 512);
  if (!mesh) mesh = make_shared<Mesh>(shader, "asteroid.obj");
}

Asteroid::~Asteroid() {
}

bool Asteroid::Update(Scene &scene, float dt) {
  // Count time alive
  age += dt;

  // Animate position according to time
  position += speed * dt;

  // Rotate the object
  rotation += rotMomentum * dt;

  // Delete when alive longer than 10s or out of visibility
  if (age > 10.0f || position.y < -10) return false;

  // Collide with scene
  for (auto &obj : scene.objects) {
    // Ignore self in scene
    if (obj.get() == this) continue;

    // We only need to collide with asteroids and projectiles, ignore other objects
    auto asteroid = dynamic_pointer_cast<Asteroid>(obj);
    auto projectile = dynamic_pointer_cast<Projectile>(obj);
    if (!asteroid && !projectile) continue;

    // When colliding with other asteroids make sure the object is older than .5s
    // This prevents excessive collisions when asteroids explode.
    if (asteroid && age < 0.5f) continue;

    // Compare distance to approximate size of the asteroid estimated from scale.
    if (distance(position, obj->position) < (obj->scale.y + scale.y)*0.7f) {
      int pieces = 3;

      // Too small to split into pieces
      if (scale.y < 0.5) pieces = 0;

      // The projectile will be destroyed
      if (projectile) projectile->Destroy();

      // Generate smaller asteroids
      Explode(scene, (obj->position+position)/2.0f, (obj->scale+scale)/2.0f, pieces);

      // Destroy self
      return false;
    }
  }

  // Generate modelMatrix from position, rotation and scale
  GenerateModelMatrix();

  return true;
}

void Asteroid::Explode(Scene &scene, vec3 explPosition, vec3 explScale, int pieces) {
  // Generate explosion
  auto explosion = make_shared<Explosion>();
  explosion->position = explPosition;
  explosion->scale = explScale;
  explosion->speed = speed/2.0f;
  scene.objects.push_back(explosion);

  // Generate smaller asteroids
  for (int i = 0; i < pieces; i++) {
    auto asteroid = make_shared<Asteroid>();
    asteroid->speed = speed + vec3(Rand(-3.0f, 3.0f), Rand(0.0f, -5.0f), 0.0f);;
    asteroid->position = position;
    asteroid->rotMomentum = rotMomentum;
    float factor = (float)pieces/2.0f;
    asteroid->scale = scale / factor;
    scene.objects.push_back(asteroid);
  }
}

void Asteroid::Render(Scene &scene) {
  shader->Use();

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}

