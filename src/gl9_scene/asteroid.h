#ifndef PPGSO_ASTEROID_H
#define PPGSO_ASTEROID_H

#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"

// Simple asteroid object
// This sphere object represents an instance of mesh geometry
// It initializes and loads all resources only once
// It will move down along the Y axis and self delete when reaching below -10
class Asteroid : public Object {
private:
  // Static resources (Shared between instances)
  static std::shared_ptr<ppgso::Mesh> mesh;
  static std::shared_ptr<ppgso::Shader> shader;
  static std::shared_ptr<ppgso::Texture> texture;

  // Age of the object in seconds
  float age = 0;

  // Speed and rotational momentum
  glm::vec3 speed;
  glm::vec3 rotMomentum;

  // Generate explosion on position and scale, produce N smaller asteroid pieces
  void Explode(Scene &scene, glm::vec3 explosionPosition, glm::vec3 explosionScale, int pieces);
public:
  Asteroid();
  ~Asteroid();

  // Implement object interface
  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;
private:
};

#endif //PPGSO_ASTEROID_H
