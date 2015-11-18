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

  virtual bool Update(Scene &scene, float dt);
  virtual void Render(Scene &scene);

  float time;
private:
  float Rand(float min, float max);
};
typedef std::shared_ptr< Generator > GeneratorPtr;

#endif //PPGSO_GENERATOR_H
