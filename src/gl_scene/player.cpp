#include "player.h"
#include "scene.h"
#include "asteroid.h"

#include "object_frag.h"
#include "object_vert.h"

#include <GLFW/glfw3.h>

#define PI 3.14159265358979323846f

bool Player::Update(Scene &scene, float dt) {
  // Hit detection
  for ( auto obj : scene.objects ) {
    // Ignore self in scene
    if (obj.get() == this)
      continue;

    // We only need to collide with spheres, ignore other objects
    auto sphere = std::dynamic_pointer_cast<Asteroid>(obj);
    if (!sphere) continue;

    if (glm::distance(position, sphere->position) < sphere->scale.y) {
      std::cout << "Hit object: " << sphere << std::endl;
      return false;
    }
  }

  // Keyboard controls
  if(scene.keyboard[GLFW_KEY_LEFT]) {
    position.x += 10 * dt;
    rotation.y = PI/4.0f;
  } else if(scene.keyboard[GLFW_KEY_RIGHT]) {
    position.x -= 10 * dt;
    rotation.y = -PI/4.0f;
  } else {
    rotation.y = 0;
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

Player::Player() {
  // Scale the default model
  scale *= 3.0f;
  rotation.x = PI/2.0f;

  // Initialize static resources if needed
  if (!shader) shader = ShaderPtr(new Shader{object_vert, object_frag});
  if (!texture) texture = TexturePtr(new Texture{"corsair.rgb", 256, 512});
  if (!mesh) mesh = MeshPtr(new Mesh{shader, "corsair.obj"});
}

Player::~Player() {
}

// shared resources
MeshPtr Player::mesh;
ShaderPtr Player::shader;
TexturePtr Player::texture;