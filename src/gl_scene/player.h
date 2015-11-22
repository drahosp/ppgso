#ifndef PPGSO_PLAYER_H
#define PPGSO_PLAYER_H

#include <texture.h>
#include <shader.h>
#include <mesh.h>

#include "object.h"

// Simple object representing the player
// Reads keyboard status and manipulates its own position
// On Update checks collisions with Asteroid objects in the scene
class Player : public Object {
public:
  Player();
  ~Player();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;

private:
  // Delay fire and fire rate
  float fireDelay;
  float fireRate;
  glm::vec3 fireOffset;

  // Static resources (Shared between instances)
  static MeshPtr mesh;
  static ShaderPtr shader;
  static TexturePtr texture;
};
typedef std::shared_ptr< Player > PlayerPtr;

#endif //PPGSO_PLAYER_H
