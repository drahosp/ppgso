#ifndef PPGSO_EXPLOSION_H
#define PPGSO_EXPLOSION_H

#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "object.h"

class Explosion : public Object {
public:
  Explosion();
  ~Explosion();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;

  glm::vec3 speed;
private:
  float age;
  float maxAge;
  glm::vec3 rotMomentum;

  static ShaderPtr shader;
  static MeshPtr mesh;
  static TexturePtr texture;
};
typedef std::shared_ptr< Explosion > ExplosionPtr;


#endif //PPGSO_EXPLOSION_H
