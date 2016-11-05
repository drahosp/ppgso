#ifndef PPGSO_SPACE_H
#define PPGSO_SPACE_H

#include <ppgso/ppgso.h>

#include "object.h"

// This object renders the scene background
// It does not use the camera so it uses different shader program
// Background animation is achieved by passing an offset to the fragment shader that offsets texture mapping
class Space : public Object {
private:
  // Static resources (Shared between instances)
  static std::shared_ptr<ppgso::Mesh> mesh;
  static std::shared_ptr<ppgso::Shader> shader;
  static std::shared_ptr<ppgso::Texture> texture;

  float offset;
public:
  Space();
  ~Space();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;
};

#endif //PPGSO_SPACE_H
