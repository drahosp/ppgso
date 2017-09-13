#pragma once
#include "object.h"
#include "scene.h"

/*!
 * Example generator of objects
 * Constructs a new object during Update and adds it into the scene
 * Does not render anything
 */
class Generator final : public Object {
public:
  /*!
   * Generate new asteroids over time
   * @param scene Scene to update
   * @param dt Time delta
   * @return true to delete the object
   */
  bool update(Scene &scene, float dt) override;

  /*!
   * Render placeholder for generator
   * @param scene Scene to render in
   */
  void render(Scene &scene) override;

  float time = 0.0f;
};
