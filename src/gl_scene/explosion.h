#ifndef PPGSO_EXPLOSION_H
#define PPGSO_EXPLOSION_H

#include <ppgso/ppgso.h>

#include "object.h"

class Explosion : public Object {
private:
  static std::shared_ptr<ppgso::Shader> shader;
  static std::shared_ptr<ppgso::Mesh> mesh;
  static std::shared_ptr<ppgso::Texture> texture;

  float age = 0;
  float maxAge = 0.2f;
  glm::vec3 rotMomentum;
public:
  glm::vec3 speed;

  Explosion();
  ~Explosion();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;
};

#endif //PPGSO_EXPLOSION_H
