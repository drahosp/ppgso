#ifndef PPGSO_SPHERE_H
#define PPGSO_SPHERE_H

#include <memory>

#include <GL/glew.h>

#include "scene.h"
#include "object.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

// Simple asteroid object
// This sphere object represents an instance of mesh geometry
// It initializes and loads all resources only once
// It will move down along the Y axis and self delete when reaching below -10
class Asteroid : public Object {
public:
  Asteroid();
  ~Asteroid();

  // Implement object interface
  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;
private:
  // Generate explosion on position and scale, produce N smaller asteroid pieces
  void Explode(Scene &scene, glm::vec3 explPosition, glm::vec3 explScale, int pieces);

  // Age of the object in s
  float age;

  // Speed and rotational momentum
  glm::vec3 speed;
  glm::vec3 rotMomentum;

  // Static resources (Shared between instances)
  static MeshPtr mesh;
  static ShaderPtr shader;
  static TexturePtr texture;
};
typedef std::shared_ptr<Asteroid> AsteroidPtr;

#endif //PPGSO_SPHERE_H
