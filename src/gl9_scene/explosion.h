#pragma once
#include <ppgso/ppgso.h>

#include "object.h"

/*!
 * Simple explosion object that will render expanding transparent geometry in the scene with additive blending
 */
class Explosion final : public Object {
private:
  static std::unique_ptr<ppgso::Shader> shader;
  static std::unique_ptr<ppgso::Mesh> mesh;
  static std::unique_ptr<ppgso::Texture> texture;

  float age{0.0f};
  float maxAge{0.2f};
  glm::vec3 rotMomentum;
public:
  glm::vec3 speed;

  /*!
   * Create new Explosion
   */
  Explosion();

  /*!
   * Update explosion
   * @param scene Scene to update
   * @param dt Time delta
   * @return true to delete the object
   */
  bool update(Scene &scene, float dt) override;

  /*!
   * Render explosion
   * @param scene Scene to render in
   */
  void render(Scene &scene) override;
};

