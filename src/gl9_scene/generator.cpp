#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>

#include "generator.h"
#include "asteroid.h"

using namespace std;
using namespace glm;
using namespace ppgso;

bool Generator::update(Scene &scene, float dt) {
  // Accumulate time
  time += dt;

  // Add object to scene when time reaches certain level
  if (time > .3) {
    auto obj = make_unique<Asteroid>();
    obj->position = position;
    obj->position.x += linearRand(-20.0f, 20.0f);
    scene.objects.push_back(move(obj));
    time = 0;
  }

  return true;
}

void Generator::render(Scene &scene) {
  // Generator will not be rendered
}
