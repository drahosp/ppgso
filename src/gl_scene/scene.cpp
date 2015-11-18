#include "scene.h"

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::Update(float time) {
  camera->Update();

  // Use iterator to update all objects so we can remove while iterating
  auto i = std::begin(objects);

  while (i != std::end(objects)) {
    // Update and remove from list if needed
    auto obj = i->get();
    if (!obj->Update(*this, time))
      i = objects.erase(i); // NOTE: no need to call destructors as we store shared pointers in the scene
    else
      ++i;
  }
}

void Scene::Render() {
  // Simply render all objects
  for (auto obj : objects )
    obj->Render(*this);
}

