#include <glm/gtc/random.hpp>
#include <random>
#include <ppgso/ppgso.h>

#include "generator.h"
#include "asteroid.h"

using namespace std;
using namespace glm;
using namespace ppgso;

bool Generator::Update(Scene &scene, float dt) {
  // Accumulate time
  time += dt;

  // Add object to scene when time reaches certain level
  if (time > .3) {
    auto obj = make_shared<Asteroid>();
    obj->position = position;
    obj->position.x += linearRand(-20.0f, 20.0f);
    scene.objects.push_back(obj);
    time = 0;
  }

  return true;
}

void Generator::Render(Scene &scene) {
  // Generator will not be rendered
}
