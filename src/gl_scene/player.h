#ifndef PPGSO_PLAYER_H
#define PPGSO_PLAYER_H

#include <ppgso/ppgso.h>

#include "object.h"

// Simple object representing the player
// Reads keyboard status and manipulates its own position
// On Update checks collisions with Asteroid objects in the scene
class Player : public Object {
private:
  // Static resources (Shared between instances)
  static std::shared_ptr<ppgso::Mesh> mesh;
  static std::shared_ptr<ppgso::Shader> shader;
  static std::shared_ptr<ppgso::Texture> texture;

  // Delay fire and fire rate
  float fireDelay;
  float fireRate;
  glm::vec3 fireOffset;

public:
  Player();
  ~Player();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;
};

#endif //PPGSO_PLAYER_H
