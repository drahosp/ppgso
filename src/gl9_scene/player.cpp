#include "player.h"
#include "scene.h"
#include "asteroid.h"
#include "projectile.h"
#include "explosion.h"

#include "object_frag.h"
#include "object_vert.h"

#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
shared_ptr<Mesh> Player::mesh;
shared_ptr<Texture> Player::texture;
shared_ptr<Shader> Player::shader;

Player::Player() {
  // Reset fire delay
  fireDelay = 0;
  // Set the rate of fire
  fireRate = 0.3f;
  // Fire offset;
  fireOffset = {0.7f,0.0f,0.0f};

  // Scale the default model
  scale *= 3.0f;

  // Initialize static resources if needed
  if (!shader) shader = make_shared<Shader>(object_vert, object_frag);
  if (!texture) texture = make_shared<Texture>("corsair.bmp");
  if (!mesh) mesh = make_shared<Mesh>(shader, "corsair.obj");
}

Player::~Player() {
}

bool Player::Update(Scene &scene, float dt) {
  // Fire delay increment
  fireDelay += dt;

  // Hit detection
  for ( auto obj : scene.objects ) {
    // Ignore self in scene
    if (obj.get() == this)
      continue;

    // We only need to collide with asteroids, ignore other objects
    auto asteroid = std::dynamic_pointer_cast<Asteroid>(obj);
    if (!asteroid) continue;

    if (glm::distance(position, asteroid->position) < asteroid->scale.y) {
      // Explode
      auto explosion = make_shared<Explosion>();
      explosion->position = position;
      explosion->scale = scale * 3.0f;
      scene.objects.push_back(explosion);

      // Die
      return false;
    }
  }

  // Keyboard controls
  if(scene.keyboard[GLFW_KEY_LEFT]) {
    position.x += 10 * dt;
    rotation.z = -PI/4.0f;
  } else if(scene.keyboard[GLFW_KEY_RIGHT]) {
    position.x -= 10 * dt;
    rotation.z = PI/4.0f;
  } else {
    rotation.z = 0;
  }

  // Firing projectiles
  if(scene.keyboard[GLFW_KEY_SPACE] && fireDelay > fireRate) {
    // Reset fire delay
    fireDelay = 0;
    // Invert file offset
    fireOffset = -fireOffset;

    auto projectile = make_shared<Projectile>();
    projectile->position = position + glm::vec3(0.0f, 0.0f, 0.3f) + fireOffset;
    scene.objects.push_back(projectile);
  }

  GenerateModelMatrix();
  return true;
}

void Player::Render(Scene &scene) {
  shader->Use();

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}
