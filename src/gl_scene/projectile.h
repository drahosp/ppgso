#ifndef PPGSO_PROJECTILE_H
#define PPGSO_PROJECTILE_H

#include "shader.h"
#include "mesh.h"
#include "object.h"

class Projectile : public Object {
public:
  Projectile();
  ~Projectile();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;

  void Destroy();
private:
  float age;
  glm::vec3 speed;
  glm::vec3 rotMomentum;

  static ShaderPtr shader;
  static MeshPtr mesh;
  static TexturePtr texture;
};
typedef std::shared_ptr< Projectile > ProjectilePtr;

#endif //PPGSO_PROJECTILE_H
