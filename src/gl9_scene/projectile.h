#pragma once
#include <ppgso/ppgso.h>

#include "object.h"

/*!
 * Object representing a rocket projectile that will accelerate from the ship one created
 */
class Projectile final : public Object {
private:
  static std::unique_ptr<ppgso::Shader> shader;
  static std::unique_ptr<ppgso::Mesh> mesh;
  static std::unique_ptr<ppgso::Texture> texture;

  float age{0.0f};
  glm::vec3 speed;
  glm::vec3 rotMomentum;
public:
  /*
   * Create new projectile
   */
  Projectile();

  /*!
   * Update projectile position
   * @param scene Scene to update
   * @param dt Time delta
   * @return true to delete the object
   */
  bool update(Scene &scene, float dt) override;

  /*!
   * Render projectile
   * @param scene Scene to render in
   */
  void render(Scene &scene) override;

  /*!
   * Destroy the projectile
   */
  void destroy();
};

