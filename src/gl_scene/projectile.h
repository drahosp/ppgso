#ifndef PPGSO_PROJECTILE_H
#define PPGSO_PROJECTILE_H

#include <ppgso/ppgso.h>

#include "object.h"

class Projectile : public Object {
private:
  static std::shared_ptr<ppgso::Shader> shader;
  static std::shared_ptr<ppgso::Mesh> mesh;
  static std::shared_ptr<ppgso::Texture> texture;

  float age = 0;
  glm::vec3 speed;
  glm::vec3 rotMomentum;
public:
  Projectile();
  ~Projectile();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;

  void Destroy();
};

#endif //PPGSO_PROJECTILE_H
