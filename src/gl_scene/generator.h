#ifndef PPGSO_GENERATOR_H
#define PPGSO_GENERATOR_H

#include <random>
#include "object.h"
#include "scene.h"

// Example generator of objects
// Constructs a new object during Update and adds it into the scene
// Does not render anything
class Generator : public Object {
public:
  Generator();
  ~Generator();

  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;

  float time;
};
typedef std::shared_ptr< Generator > GeneratorPtr;

#endif //PPGSO_GENERATOR_H
