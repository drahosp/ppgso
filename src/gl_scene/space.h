#ifndef PPGSO_SPACE_H
#define PPGSO_SPACE_H

#include "mesh.h"
#include "object.h"

// This object renders the scene background
// It does not use the camera so it uses different shader program
// Background animation is achieved by passing an offset to the fragment shader that offsets texture mapping
class Space : public Object {
public:
  Space();
  ~Space();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;

private:
  float offset;
  // Static resources (Shared between instances)
  static MeshPtr mesh;
  static ShaderPtr shader;
  static TexturePtr texture;
};
typedef std::shared_ptr< Space > SpacePtr;

#endif //PPGSO_SPACE_H
